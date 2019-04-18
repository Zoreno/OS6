#include <pci/pci_io.h>

#include <arch/arch.h>

#define MAKE_ADDR(id, reg) (0x80000000 | id | (reg & 0xFC))

#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC

uint8_t pci_read_b(uint32_t id, uint32_t reg)
{
    uint32_t addr = MAKE_ADDR(id, reg);

    outportl(PCI_CONFIG_ADDR, addr);

    return inportb(PCI_CONFIG_DATA + (reg & 0x03));
}

uint16_t pci_read_w(uint32_t id, uint32_t reg)
{
    uint32_t addr = MAKE_ADDR(id, reg);

    outportl(PCI_CONFIG_ADDR, addr);

    return inportw(PCI_CONFIG_DATA + (reg & 0x02));
}

uint32_t pci_read_l(uint32_t id, uint32_t reg)
{
    uint32_t addr = MAKE_ADDR(id, reg);

    outportl(PCI_CONFIG_ADDR, addr);

    return inportl(PCI_CONFIG_DATA);
}

void pci_write_b(uint32_t id, uint32_t reg, uint8_t data)
{
    uint32_t addr = MAKE_ADDR(id, reg);

    outportl(PCI_CONFIG_ADDR, addr);

    outportb(PCI_CONFIG_DATA + (reg & 0x03), data);
}

void pci_write_w(uint32_t id, uint32_t reg, uint16_t data)
{
    uint32_t addr = MAKE_ADDR(id, reg);

    outportl(PCI_CONFIG_ADDR, addr);

    outportw(PCI_CONFIG_DATA + (reg & 0x02), data);
}

void pci_write_l(uint32_t id, uint32_t reg, uint32_t data)
{
    uint32_t addr = MAKE_ADDR(id, reg);

    outportl(PCI_CONFIG_ADDR, addr);

    outportl(PCI_CONFIG_DATA, data);
}