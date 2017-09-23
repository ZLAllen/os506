#include <sys/pci.h>
#include <sys/system.h>
#include <sys/kprintf.h>
#include <sys/ahci.h>

#define SATA_SIG_ATAPI 0xEB140101
#define SATA_SIG_SEMB  0xC33C0101
#define SATA_SIG_PM    0x96690101
#define HBA_PORT_DET_PRESENT 3
#define HBA_PORT_IPM_ACTIVE  1

/* Walk the configuration space */

int pciCheckVendor(uint8_t bus, uint8_t slot);
uint32_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void picConfigWriteWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void probe_port(hba_mem_t* abar);
static int check_type(hba_port_t* port);


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
    int i, j;
    uint32_t tmp;
    //uint8_t class;
    //uint8_t subclass;
    //uint8_t prog;

    for(i = 0; i < 256; i++)
    {
        for(j = 0; j < 32; j++)
        {
            if(pciCheckVendor((uint8_t)i,(uint8_t)j) == 0)
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
                    kprintf("%x\n", ((hba_mem_t*)(0xffffffff80000000 + 0x20C000))->pi);
                    probe_port((hba_mem_t*)((uint64_t)(0xffffffff80000000 + 0x20C000)));
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

    out32(0xCFC,(uint32_t)0x0020C000);


}


int pciCheckVendor(uint8_t bus, uint8_t slot)
{
    uint32_t reg;

    if((reg = pciConfigReadWord(bus, slot, 0, 0))!= 0xFFFFFFFF)
    {
        kprintf("Bus %d, Device %d\n", bus, slot);
        //kprintf("Vendor ID is %x\n", reg & 0xFFFF);
        //kprintf("Device ID is %x\n", (reg >> 16) & 0xFFFF);
        kprintf("Reg is %x\n", reg);
        return 0;
    }
    return -1;
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

   

