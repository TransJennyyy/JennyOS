#include "PCI.hpp"

#include "../io.hpp"
#include "../console/console.hpp"
#include "../Apic/Apic.hpp"



unsigned int PCI::ReadPCI(unsigned char bus, unsigned char device, unsigned char func, unsigned char offset)
{
    
    unsigned int BusL = (unsigned int)bus;
    unsigned int DevL = (unsigned int)device;
    unsigned int FuncL = (unsigned int)func;
    unsigned int OffsetL = (unsigned int)offset;


    unsigned int Address = (BusL << 16) | (DevL << 11) | (FuncL << 8) | offset | (unsigned int)0x80000000;

    IO::outl(CONFIG_ADDRESS, Address);

    return IO::inl(CONFIG_DATA);
    

    
    
}

void PCI::WritePCI(unsigned char bus, unsigned char device, unsigned char func, unsigned char offset, unsigned int data)
{
    unsigned int BusL = (unsigned int)bus;
    unsigned int DevL = (unsigned int)device;
    unsigned int FuncL = (unsigned int)func;
    unsigned int OffsetL = (unsigned int)offset;


    unsigned int Address = (BusL << 16) | (DevL << 11) | (FuncL << 8) | offset | (unsigned int)0x80000000;

    IO::outl(CONFIG_ADDRESS, Address);

    IO::outl(CONFIG_DATA, data);
  

}