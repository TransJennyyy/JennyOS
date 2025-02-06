#include "PCI.hpp"
#include "../Apic/Apic.hpp"
#include "../Apic/Madt.hpp"
#include "../console/console.hpp"
#include "../malloc/malloc.hpp"


unsigned int PCI::NumberPCIDevices;

void PCI::FindDevices()
{
   ScanPCI();

}

struct PciDevice* PCI::PciDevs;
void PCI::ScanPCI()
{
    
    Console::PrintString("Starting Scan Of PCI Devices...");
    NumberPCIDevices = 0;
    unsigned int HighistBus = 0;
    for(int bus=0; bus< 256; bus++)
    {
        for(int device=0; device < 32; device++ )
        {
            for(int func =0; func < 8; func++)
            {
                unsigned int DeviceID = ReadPCI(bus, device, func, 0);
                if((DeviceID & 0xffff) != 0xffff)
                {
                    NumberPCIDevices++;
                    HighistBus = bus;
                }
            }
        }
    }
    Console::PrintString("Done\n");
    Console::PrintString("Found ");
    Console::PrintInt(NumberPCIDevices, 0);
    Console::PrintString(" PCI Devices\n");

    PciDevs = (struct PciDevice*) Kmalloc::Malloc(sizeof(struct PciDevice)*NumberPCIDevices);
    if(PciDevs == 0)
    {
        Console::PrintString("Cant Alloc Memory For Pci Device\n");
        asm("cli\nhlt");
    }
    unsigned int DevIndex = 0;

    for(int bus=0; bus< HighistBus; bus++)
    {
        for(int device=0; device < 32; device++ )
        {
            for(int func =0; func < 8; func++)
            {
                unsigned int DeviceID = ReadPCI(bus, device, func, 0);
                if((DeviceID & 0xffff) != 0xffff)
                {
                    unsigned int ClassTmp = ReadPCI(bus,device, func, 0x08);
                    unsigned int ClassCode = (((ClassTmp >> 24) & 0xff)) | (((ClassTmp >> 16) & 0xff) << 8) | (((ClassCode >> 8) & 0xff) << 16);
                    struct PciDevice PciDev = {0};
                    PciDev.ClassCode = ClassCode;
                    PciDev.DeviceIndex = DevIndex;
                    PciDev.DeviceID = DeviceID;
                    PciDev.DeviceLocasion = bus | (device << 8) | (func << 16);
                    PciDevs[DevIndex] = PciDev;
                }
            }
        }
    }
    Console::PrintString("Saved Device Data\n");
    FindKnownDrivers();


}