#include <sys/ahci.h>
#include <sys/pci.h>
#include <sys/system.h>
#include <sys/kprintf.h>

static int check_type(hba_port_t* port);

static void start_cmd(hba_port_t* port);
static void stop_cmd(hba_port_t* port);
static int find_cmdslot(hba_port_t* port);

int write(hba_port_t* port, uint32_t startl, uint32_t starth, uint32_t count, uint64_t buf)
{

    port->is_rwc = (int)-1;
    //port->serr_rwc &= ~(1 << 16);
/*
    kprintf("write status: %x\n", port->is_rwc);
    kprintf("ssts: %x\n", port->ssts);
    kprintf("pxcmd %x\n", port->cmd);
    */
    uint32_t spin = 0;
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
        cmdtbl->prdt_entry[i].dbc = 16*512-1;
        cmdtbl->prdt_entry[i].i = 1;
        ptr += 16*512; 
        count -= 16;
    }

    // last entry
    cmdtbl->prdt_entry[i].dba = (uint64_t)ptr;
    cmdtbl->prdt_entry[i].dbc = (count << 9)-1;
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

    kprintf("write flags debug:\n");
    kprintf("ssts: %x\n", port->ssts);
    kprintf("serr: %x\n", port->serr_rwc);
    kprintf("sact: %x\n", port->sact);
    kprintf("cmd: %x\n", port->cmd);
    kprintf("PxIS: %x\n", port->is_rwc);
    kprintf("tfd: %x\n", port->tfd);
   


    while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 3000000)
    {
        spin++;
    }

    if(spin == 3000000)
    {
        kprintf("Port is hung\n");
        return 0;
    }
  

    port->ci = 1<<slot;

    while(1)
    {
        if((port->ci & (1 << slot)) == 0)
       // if(port->is_rwc & (1 << 5))
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
    uint32_t spin = 0;
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
        cmdtbl->prdt_entry[i].dbc = 16*512-1;
        cmdtbl->prdt_entry[i].i = 0;
        ptr += 16*512; 
        count -= 16;
    }

    // last entry
    cmdtbl->prdt_entry[i].dba = (uint64_t)ptr;
    cmdtbl->prdt_entry[i].dbc = (count << 9)-1;
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

    while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 3000000)
    {
        spin++;
    }

    if(spin == 3000000)
    {
        kprintf("Port is hung\n");
        return 0;
    }
  

    port->ci = 1<<slot;

    kprintf("read status: %x\n", port->is_rwc);
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

   // kprintf("pxcmd %x\n", port->cmd);
    stop_cmd(port);

  //  kprintf("pxcmd %x\n", port->cmd);

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

   // kprintf("pxcmd %x\n", port->cmd);
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

    while(port->cmd & HBA_PxCMD_CR);

    port->cmd &= ~HBA_PxCMD_FRE;

    while(port->cmd & HBA_PxCMD_FR);
    
    /*
    while(1)
    {
        if(port->cmd & HBA_PxCMD_FR)
            continue;
        if(port->cmd & HBA_PxCMD_CR)
            continue;
        break;
    }
    */

}

static int check_type(hba_port_t* port)
{
    //uint32_t i = 0;

    /*
    uint32_t ssts = port->ssts;
   
    uint8_t det = ssts & 0x0F;

    uint8_t ipm = (port->ssts >> 8) & 0x0F;
    */
    uint8_t det, ipm;


/*
    //port reset

    stop_cmd(port);

    kprintf("port->tfd: %x\n", port->tfd);
    kprintf("port->is: %x\n", port->is_rwc);

    // trigger reset
    port->sctl = 0x1;
    while(i < 100000){
        ++i;            //wait
    }
    // stop reset
    port->sctl = 0;
    while((det = (port->ssts & 0x0F)) != 0x3); // wait for reset packet
    port->serr_rwc = (int)-1; // clear interrupt flags
   
   // kprintf("serr: %x\n", port->serr_rwc);

   // kprintf("ssts is %x\n",port->ssts);
   // kprintf("sig is %x\n", port->sig);

    //disable sleep mode
    port->sctl = (7 << 8);

   // port->cmd |= HBA_PxCMD_ST;

    
    //software reset

  // port->cmd &= ~(1 << 0);

    // allow spin
    port->cmd |= (1 << 1);

  //  port->cmd |= (1 << 0);

   // kprintf("new port tfd is %x\n", port->tfd);
*/
    ipm = (port->ssts >> 8)& 0x0F;
    det = port->ssts & 0x0F;

    if(det != HBA_PORT_DET_PRESENT)
    {
       // kprintf("port not present\n");
        return 0;
    }
    if(ipm != HBA_PORT_IPM_ACTIVE)
    {
       // kprintf("port not active\n");
        return 0;
    }

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
    uint32_t pi;
    int i = 0;
    int dt;



    //abar init
    abar->cap &= ~(1 << 26); //disable aggressive power
    abar->cap &= ~(1 << 14);

    kprintf("abar->cap: %x\n", abar->cap);
   

    //HBA reset
    abar->ghc |= (1 << 31);

    abar->ghc |= (1 << 0);

    while(abar->ghc & (1 << 0));
   
    

    pi = abar->pi;
    kprintf("start probing\n");

    while(i < 32)
    {
        if(pi&1)
        {
            dt = check_type(&(abar->ports[i]));
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

void ahciTest()
{
    char* buf1 = (char*)BUF_BASE;
    //char* buf1 = (char*)0x800000;
    char* buf2 = buf1 + 4*1024;
    char* ptr;
    int error = 0, flag = 0;
    int k;

    pci_device_t ahci;
    ahci.vendor = (uint16_t)VENDOR_INTEL;
    ahci.device = (uint16_t)DEVICE_AHCI;
    ahci.type = (uint8_t)CLASS_STORAGE;
    ahci.subtype = (uint8_t)SUBCLASS_SATA;
    ahci.prog = (uint8_t)PROG_AHCI;
    ahci.header = (uint8_t)0;

    if(!pciWalk_bf(&ahci))
    {
        kprintf("no device found\n");
    }


    hba_mem_t* abar = (hba_mem_t*)((uint64_t)(ahci.bar5));
   // kprintf("pi is %x\n", abar->pi);
    //kprintf("version is %x\n", abar->vs);
    probe_port(abar);
    port_rebase(&(abar->ports[0]), 0);

    for(k = 0; k < 100; ++k)
    {
        memset(buf1, k, 4*1024);
        if(!write(&abar->ports[0],8*k,0,8,(uint64_t)buf1))
            kprintf("Write data failed\n");
    }

    for(k = 0; k < 100; ++k)
    {
        memset(buf2, 0, 4*1024);
        if(!read(&abar->ports[0],8*k,0, 8, (uint64_t)buf2))
            kprintf("Read data failed\n");


        ptr = buf2;
        flag = 0;
        
        while(ptr < buf2 + 4*1024)
        {
            if(*ptr != k)
            {
                error += 1;
                flag = 1;
                kprintf("Read byte %d failed\n", k);
                break;
            }
            ++ptr;
        }
        if(!flag)
            kprintf("Read byte %d succeeds\n", k);

    }

    if(error)
        kprintf("Read test failed %d times\n", error);
    else
        kprintf("all read tests passed\n");

}
