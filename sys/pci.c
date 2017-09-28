#include <sys/pci.h>
#include <sys/system.h>
#include <sys/kprintf.h>
#include <sys/ahci.h>



/* Walk the configuration space */

int pciCheckModel(uint8_t bus, uint8_t slot, uint32_t model);
uint32_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void picConfigWriteWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t data);
/* Move to AHCI.c later
 *
 *
 * 
 * */
int pciWalk_bf(pci_device_t* pci)
{
    int i, j, k; 
    uint32_t tmp;
   // uint32_t header = ((uint32_t)(pci->header) << 16);
    uint32_t class = ((uint32_t)(pci->type) << 24);
    uint32_t subclass = ((uint32_t)(pci->subtype) << 16);
    uint32_t prog = ((uint32_t)(pci->prog) << 8);

    kprintf("walk start\n");

    for(i = 0; i < 256; i++)
    {
        for(j = 0; j < 32; j++)
        {
            if(pciCheckModel((uint8_t)i,(uint8_t)j, ((uint32_t)pci->device) << 16| pci->vendor))
            {
                //check header first
                tmp = pciConfigReadWord((uint8_t)i,(uint8_t)j, 0, 0x0C);
              /*  if((tmp & 0x00FF0000) != header)
                    continue;
                    */
                if(tmp & 0x00800000)
                {
                    k = 8;
                }
                else
                {
                    k = 1;
                }

                while(k >= 0)
                {

                    tmp = pciConfigReadWord((uint8_t)i,(uint8_t)j, k, 0x00);
                    if(tmp != 0xFFFFFFFF)
                    {
                       // kprintf("Multi-func devices: %p\n",tmp);
                        //check class and subclass
                        tmp = pciConfigReadWord((uint8_t)i,(uint8_t)j, (uint8_t)k, 0x08);
                        if((tmp & 0xFFFFFF00) == (class|subclass|prog))
                        {
                            tmp = pciConfigReadWord((uint8_t)i,(uint8_t)j, (uint8_t)k, 0x00);
                            kprintf("device name: %x\n", tmp);
                            break;
                        }
                        else
                            --k;
                    }
                    else
                    {
                        --k;
                    }

                }
               // kprintf("Class Code: %x\n", class);
               // kprintf("Subclass: %x\n", subclass);
               // kprintf("Prog IF: %x\n",prog);
               if(k < 0)
                   continue;
                
                //tmp = pciConfigReadWord((uint8_t)i,(uint8_t)j, 0, 0x24);
                //kprintf("base reg 5: %p\n",tmp);

               // tmp = pciConfigReadWord((uint8_t)i,(uint8_t)j, k, 0x24);
               // kprintf("base reg 5: %p\n",tmp);
                
                picConfigWriteWord((uint8_t)i,(uint8_t)j, (uint8_t)k, 0x24, (uint32_t)ABAR_BASE);

                pci->bar5 = ABAR_BASE;
               
                tmp = pciConfigReadWord((uint8_t)i,(uint8_t)j, (uint8_t)k, 0x3C);
              //  kprintf("Interrupt: %p\n",tmp);

                pci->interrupt = (uint8_t)(tmp & 0xFF);


               // tmp = pciConfigReadWord((uint8_t)i,(uint8_t)j, k, 0x24);
               // kprintf("new reg 5: %p\n",tmp);

                //continue;
                return 1;
            }
        }
    }

    return 0;
}

void picConfigWriteWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t data)
{

    uint32_t addr;
    
    addr = ((uint32_t)bus << 16) | ((uint32_t)slot << 11) | ((uint32_t)func << 8) | 
        (offset & 0xfc) | (0x80000000);

    out32(0xCF8, addr);

    out32(0xCFC,(uint32_t)data);
}


int pciCheckModel(uint8_t bus, uint8_t slot, uint32_t model)
{
    uint32_t reg;

    //if((reg = pciConfigReadWord(bus, slot, 0, 0)) != 0xFFFFFFFF)
    if(((reg = pciConfigReadWord(bus, slot, 0, 0)) & 0xFFFF)  != 0xFFFF)
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

   

