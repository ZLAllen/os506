#include <sys/pci.h>
#include <sys/system.h>
#include <sys/kprintf.h>
#include <sys/ahci.h>


/* Walk the configuration space */

int pciCheckVendor(uint8_t bus, uint8_t slot);
uint32_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

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
                    kprintf("%x\n", ((hba_mem_t*)(0xFFFFFFFF00000000+(uint64_t)tmp))->pi);
                }

            }
        }
    }

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

   

