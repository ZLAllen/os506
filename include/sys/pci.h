#ifndef _PCI_H_
#define _PCI_H_

#include <sys/defs.h>

#define VENDOR_INTEL 0x8086
#define DEVICE_AHCI 0x2922
#define CLASS_STORAGE 0x01
#define SUBCLASS_SATA 0x06
#define PROG_AHCI 0x01

#define ABAR_BASE 0xA6000
#define BUF_BASE 0x30C000
//#define ABAR_BASE 0x30000000
typedef struct {
    uint16_t vendor;
    uint16_t device;
    uint8_t type;
    uint8_t subtype;
    uint8_t prog;
    uint8_t header;
    uint32_t bar5;
    uint8_t interrupt;
} pci_device_t;

int pciWalk_bf(pci_device_t*);

#endif
