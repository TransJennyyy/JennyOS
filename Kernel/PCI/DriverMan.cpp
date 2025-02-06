#include "PCI.hpp"
#include "../Disk/ATA.hpp"
#include "../console/console.hpp"
#include "../malloc/malloc.hpp"

DiskReadDriverType PCI::BootDiskRead;
DiskReadDriverType PCI::BootDiskWrite;
extern "C" void BackUpSystemInit();
extern void DiskReadFuncWrapper(unsigned int WantedAddr, unsigned int NumberSectors, unsigned int Sector);
extern void DiskWriteFuncWrapper(unsigned int WantedAddr, unsigned int NumberSectors, unsigned int Sector);
#define NumberDrivers 2
void PCI::FindKnownDrivers()
{
    BootDiskRead = 0;
    BootDiskWrite = 0;
    DriverFunc DriverIDs[NumberDrivers] = { ATA::CheckIfUsable, ATA::CheckIfUsable };
    unsigned int ClassCodes[NumberDrivers] = { ATADriverClass, IDRDriverClass };
    if(NumberPCIDevices == 0){
        return;
    }
    if(PciDevs == 0)
    {
        return;
    }

    for(int i=0; i< NumberPCIDevices; i++)
    {
        for(int j =0; j< NumberDrivers; j++){
            if(((ClassCodes[j] >> 16) & 0xff) != 0xff){
                if(PciDevs[i].ClassCode == ClassCodes[j])
                {
                    if(!DriverIDs[j]())
                    {
                        Console::PrintString("Driver Not usable\n");
                        continue;
                    }
                    Console::PrintString("Found Device\n");
                    break; 
                }
                continue;
            }
            if((PciDevs[i].ClassCode & 0xffff) == (ClassCodes[j] & 0xffff))
            {
                Console::PrintString("Found Device\n");
                if(!DriverIDs[j]())
                {
                    Console::PrintString("Driver Not usable\n");
                    continue;
                }
                continue;
            }


        }
    }
    Console::PrintString("Scanned Devs\n");
    if(BootDiskRead == 0 || BootDiskWrite == 0)
    {
        Console::PrintString("Error: No drivers found to load data\n");
        Console::PrintString("Attempting Backup solusion (Warning Will be slow asf)\n");
        BackUpSystemInit();
        PCI::BootDiskRead = (DiskReadDriverType)DiskReadFuncWrapper;
        PCI::BootDiskWrite = DiskWriteFuncWrapper;
        char* DiskReadTest = (char*) Kmalloc::Malloc(512);
        PCI::BootDiskRead((unsigned int)DiskReadTest, 1, 2);
        const char* KernelMagicBytes = "JOSK";
        for(int i=0; i< 4; i++)
        {
            if(DiskReadTest[i] != KernelMagicBytes[i])
            {
                Console::PrintString("Backup Driver Check Failed, KERNEL PANIC\n");
                asm("cli\nhlt");
            }
        }
        Kmalloc::Free(DiskReadTest);
        Console::PrintString("Backup Driver Funcsional\n");
    }

}