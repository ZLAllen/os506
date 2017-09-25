#include <sys/pci.h>
#include <sys/system.h>
#include <sys/kprintf.h>
#include <sys/ahci.h>


#define SATA_SIG_ATAPI 0xEB140101
#define SATA_SIG_SEMB  0xC33C0101
#define SATA_SIG_PM    0x96690101
#define HBA_PORT_DET_PRESENT 3
#define HBA_PORT_IPM_ACTIVE  1
#define AHCI_BASE 0x400000
#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_CMD_WRITE_DMA_EX 0x35
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08

/* Walk the configuration space */

int pciCheckVendor(uint8_t bus, uint8_t slot);
uint32_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void picConfigWriteWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void probe_port(hba_mem_t* abar);
void port_rebase(hba_port_t* port, int portno);
static int check_type(hba_port_t* port);
void start_cmd(hba_port_t* port);
void stop_cmd(hba_port_t* port);
int read(hba_port_t* port, uint32_t startl, uint32_t starth, uint32_t count, uint64_t buf);
int write(hba_port_t* port, uint32_t startl, uint32_t starth, uint32_t count, uint64_t buf);
int find_cmdslot(hba_port_t* port);
/* Move to AHCI.c later
 *
 *
 * 
 * */
int write(hba_port_t* port, uint32_t startl, uint32_t starth, uint32_t count, uint64_t buf)
{
    port->is_rwc = 0xFFFFFFFF;
    int spin = 0;
    int slot = find_cmdslot(port);
    int i = 0;
    char* ptr = (char*)buf;

    if(slot < 0)
        return -1;

    //kprintf("write slot is %d\n", slot);

    hba_cmd_header_t* cmdheader = (hba_cmd_header_t*)port->clb;
    cmdheader += slot;
    cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);
    cmdheader->w = 1;
    cmdheader->c = 1;
    cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;  //16 sector per prdt;

    hba_cmd_tbl_t* cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t)+(cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));

    //16 sectors per PRDT
    for(i = 0; i < cmdheader->prdtl-1; i++)
    {
        cmdtbl->prdt_entry[i].dba = (uint64_t)ptr;
        cmdtbl->prdt_entry[i].dbc = 16*512;
        cmdtbl->prdt_entry[i].i = 0;
        ptr += 16*512; 
        count -= 16;
    }

    // last entry
    cmdtbl->prdt_entry[i].dba = (uint64_t)ptr;
    cmdtbl->prdt_entry[i].dbc = count << 9;
    cmdtbl->prdt_entry[i].i = 0;

    //set up command
    fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;
    cmdfis->command = ATA_CMD_WRITE_DMA_EX;

    cmdfis->lba0 = (uint8_t)startl;
    cmdfis->lba1 = (uint8_t)(startl>>8);
    cmdfis->lba2 = (uint8_t)(startl>>16);
    cmdfis->device = 1 << 6;

    cmdfis->lba3 = (uint8_t)(startl>>24);
    cmdfis->lba4 = (uint8_t)starth;
    cmdfis->lba5 = (uint8_t)(starth>>8);

    cmdfis->count = (uint16_t)count;

    while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
    {
        spin++;
    }

    if(spin == 1000000)
    {
        kprintf("Port is hung\n");
        return 0;
    }
    
    port->ci = 1<<slot;

    while(1)
    {
        if((port->ci & (1 << slot)) == 0)
            break;
        if(port->is_rwc & HBA_PxIS_TFES)
        {
            kprintf("write disk error\n");
            return 0;
        }
    }

    if(port->is_rwc & HBA_PxIS_TFES)
    {
        kprintf("write disk error\n");
        return 0;
    }
    
    return 1;
}


int read(hba_port_t* port, uint32_t startl, uint32_t starth, uint32_t count, uint64_t buf)
{
    port->is_rwc = 0xFFFFFFFF;
    int spin = 0;
    int slot = find_cmdslot(port);
    int i;
    char* ptr = (char*)buf;

    if(slot < 0)
        return -1;

    hba_cmd_header_t* cmdheader = (hba_cmd_header_t*)port->clb;
    cmdheader += slot;
    cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);
    cmdheader->w = 0;
    cmdheader->c = 1;
    cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;  //16 sector per prdt;

    hba_cmd_tbl_t* cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t)+(cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));

    //kprintf("%d\n", sizeof(hba_cmd_tbl_t));
    //kprintf("%d\n", sizeof(hba_prdt_entry_t));
   // kprintf("%d\n", cmdheader->prdtl-1);

    //16 sectors per PRDT
    for(i = 0; i < cmdheader->prdtl-1; i++)
    {
        cmdtbl->prdt_entry[i].dba = (uint64_t)ptr;
        cmdtbl->prdt_entry[i].dbc = 16*512;
        cmdtbl->prdt_entry[i].i = 0;
        ptr += 16*512; 
        count -= 16;
    }

    // last entry
    cmdtbl->prdt_entry[i].dba = (uint64_t)ptr;
    cmdtbl->prdt_entry[i].dbc = count << 9;
    cmdtbl->prdt_entry[i].i = 0;

    //set up command
    fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;
    cmdfis->command = ATA_CMD_READ_DMA_EX;

    cmdfis->lba0 = (uint8_t)startl;
    cmdfis->lba1 = (uint8_t)(startl>>8);
    cmdfis->lba2 = (uint8_t)(startl>>16);
    cmdfis->device = 1<<6;

    cmdfis->lba3 = (uint8_t)(startl>>24);
    cmdfis->lba4 = (uint8_t)starth;
    cmdfis->lba5 = (uint8_t)(starth>>8);

    cmdfis->count = (uint16_t)count;

    while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
    {
        spin++;
    }

    if(spin == 1000000)
    {
        kprintf("Port is hung\n");
        return 0;
    }
    
    port->ci = 1<<slot;

    while(1)
    {
        if((port->ci & (1 << slot)) == 0)
            break;
        if(port->is_rwc & HBA_PxIS_TFES)
        {
            kprintf("Read disk error\n");
            return 0;
        }
    }

    if(port->is_rwc & HBA_PxIS_TFES)
    {
        kprintf("Read disk error\n");
        return 0;
    }
    
    return 1;
}

int find_cmdslot(hba_port_t* port)
{
    uint32_t slots = (port->sact | port->ci);
    int i;
    for(i=0; i<MAX_CMD_SLOT_CNT; i++)
    {
        if((slots&1) == 0)
            return i;
        slots >>= 1;
    }

    kprintf("cannot find free command list entry\n");
    return -1;
}

void port_rebase(hba_port_t* port, int portno)
{
    int i;

    stop_cmd(port);

    // command list offset
    port->clb = ((uint64_t)(AHCI_BASE + (portno << 10)))& 0xFFFFFFFF;
    memset((void*)(port->clb), 0, 1024);

    // FIS offset
    port->fb = ((uint64_t)(AHCI_BASE + (32 << 10) + (portno << 8)))& 0xFFFFFFFF;
    memset((void*)(port->fb), 0, 256);
    
    // command table offset
    hba_cmd_header_t* cmdheader = (hba_cmd_header_t*)(port->clb);
    for(i = 0; i < 32; i++)
    {
        // 8 entries per table
        // refers to osdev table for size calculation
        cmdheader[i].prdtl = 8;
        cmdheader[i].ctba = ((uint64_t)(AHCI_BASE + (40 << 10) + (portno << 13) + (i << 8)))& 0xFFFFFFFF;
        memset((void*)cmdheader[i].ctba, 0, 256);
    }
    start_cmd(port);
}

void start_cmd(hba_port_t* port)
{
    while(port->cmd & HBA_PxCMD_CR);

    port->cmd |= HBA_PxCMD_FRE;

    port->cmd |= HBA_PxCMD_ST;

}


void stop_cmd(hba_port_t* port)
{



    port->cmd &= ~HBA_PxCMD_ST;

    port->cmd &= ~HBA_PxCMD_FRE;
    while(1)
    {
        if(port->cmd & HBA_PxCMD_FR)
            continue;
        if(port->cmd & HBA_PxCMD_CR)
            continue;
        break;
    }

}

static int check_type(hba_port_t* port)
{
    uint32_t ssts = port->ssts;

    uint8_t ipm = (ssts >> 8) & 0x0F;
    uint8_t det = ssts & 0x0F;

    if(det != HBA_PORT_DET_PRESENT)
        return 0;
    if(ipm != HBA_PORT_IPM_ACTIVE)
        return 0;

    switch(port->sig)
    {
        case SATA_SIG_ATAPI:
            return AHCI_DEV_SATAPI;
        case SATA_SIG_SEMB:
            return AHCI_DEV_SEMB;
        case SATA_SIG_PM:
            return AHCI_DEV_PM;
        default:
            return AHCI_DEV_SATA;
    }
}
void probe_port(hba_mem_t* abar)
{
    uint32_t pi = abar->pi;
    int i = 0;



    //reset ahci
    abar->ghc |= (1 << 31);
    abar->ghc |= (1 << 0);

    while(abar->ghc & (1 << 0));

    abar->ghc |= (1 << 31);


    while(i < 32)
    {
        if(pi&1)
        {
            int dt = check_type(&abar->ports[i]);
            if(dt == AHCI_DEV_SATA)
            {
                kprintf("SATA drive found at port %d\n", i);
            }
            else if(dt == AHCI_DEV_SATAPI)
            {
                kprintf("SATAPI drive found at port %d\n", i);
            }
            else if(dt == AHCI_DEV_SEMB)
            {
                kprintf("SEMB drive found at port %d\n", i);
            }
            else if(dt == AHCI_DEV_PM)
            {
                kprintf("PM drive found at port %d\n", i);
            }
            else
            {
                kprintf("No drive found at port %d\n", i);
            }

        }

        pi >>= 1;
        i++;
    }
}

void pciWalk_bf()
{
    int i, j, k, error = 0, flag = 0;
    uint32_t tmp;
    char* buf = (char*)0x30C000;
    char* ptr;
    //uint8_t class;
    //uint8_t subclass;
    //uint8_t prog;

    for(i = 0; i < 256; i++)
    {
        for(j = 0; j < 32; j++)
        {
            if(pciCheckVendor((uint8_t)i,(uint8_t)j))
            {
                tmp = pciConfigReadWord((uint8_t)i,(uint8_t)j, 0, 0x08);
                //class = (uint8_t)(tmp >> 24); 
               // subclass = (uint8_t)(tmp >> 16);
               // prog = (uint8_t)(tmp >> 8);
               // kprintf("Class Code: %x\n", class);
               // kprintf("Subclass: %x\n", subclass);
               // kprintf("Prog IF: %x\n",prog);

                // check if an AHCI controller found
                if((tmp & 0xFFFFFF00) == 0x01060100)
                {
                    tmp = pciConfigReadWord((uint8_t)i,(uint8_t)j, 0, 0x24);
                    kprintf("base reg 5: %p\n",tmp);

                    picConfigWriteWord((uint8_t)i,(uint8_t)j, 0, 0x24);
                    tmp = pciConfigReadWord((uint8_t)i,(uint8_t)j, 0, 0x24);
                    kprintf("base reg 5: %p\n",tmp);
                    tmp = pciConfigReadWord((uint8_t)i,(uint8_t)j, 0, 0x28);
                    kprintf("base reg 4: %p\n",tmp);
                    hba_mem_t* abar = (hba_mem_t*)((uint64_t)(0xffffffff80000000 + 0x30000000));
                    kprintf("%x\n", abar->pi);
                    probe_port(abar);
                    port_rebase(&abar->ports[1], 1);

					//  write 100 4kb blocks
                    for(k = 0; k < 100; ++k)
                    {
                        memset(buf, k, 4*1024);
                        if(!write(&abar->ports[1],8*k,0,8,(uint64_t)buf))
                            kprintf("write data failed\n");
                    }
					// read the data back for verification
                    for(k = 0; k < 100; ++k)
                    {
                        memset(buf, 0, 4*1024);
                        if(!read(&abar->ports[1],8*k,0, 8, (uint64_t)buf))
                            kprintf("read data failed\n");


                        ptr = buf;
                        while(ptr < buf + 4*1024)
                        {
                            flag = 0;
                            if(*ptr != k)
                            {
                                error += 1;
                                flag = 1;
                                kprintf("read byte %d failed\n", k);
                                break;
                            }
                            ++ptr;
                        }
                        if(!flag)
                            kprintf("read byte %d succeeds\n", k);

                    }

                    if(error)
                        kprintf("read test failed %d times\n", error);
                    else
                        kprintf("all read tests passed\n");
                }


            }
        }
    }

}

void picConfigWriteWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{

    uint32_t addr;
    
    addr = ((uint32_t)bus << 16) | ((uint32_t)slot << 11) | ((uint32_t)func << 8) | 
        (offset & 0xfc) | (0x80000000);

    out32(0xCF8, addr);

    out32(0xCFC,(uint32_t)0x30000000);


}


int pciCheckVendor(uint8_t bus, uint8_t slot)
{
    uint32_t reg;

    if((reg = pciConfigReadWord(bus, slot, 0, 0))!= 0xFFFFFFFF)
    {
       // kprintf("Bus %d, Device %d\n", bus, slot);
        //kprintf("Vendor ID is %x\n", reg & 0xFFFF);
        //kprintf("Device ID is %x\n", (reg >> 16) & 0xFFFF);
       // kprintf("Reg is %x\n", reg);
        return 1;
    }
    return 0;
}

uint32_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t addr;
    uint32_t tmp = 0;
    
    addr = ((uint32_t)bus << 16) | ((uint32_t)slot << 11) | ((uint32_t)func << 8) | 
        (offset & 0xfc) | (0x80000000);

    out32(0xCF8, addr);

    tmp = in32(0xCFC);
    
    return tmp;
}

   

