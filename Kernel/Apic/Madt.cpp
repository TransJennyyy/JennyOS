#include "Apic.hpp"
#include "Madt.hpp"
#include "../console/console.hpp"
#include "../io.hpp"
#include "../malloc/malloc.hpp"
#include "../Cpuid.hpp"

void* Madt::LAPICREGAddr;

void** Madt::Devices;
unsigned char* Madt::DeviceTypes;
unsigned int Madt::NumberDevices;
void Madt::AddToDeviceList(unsigned char DeviceType, void* HeaderStart)
{
    void** TmpDevices = (void**) Kmalloc::Malloc((NumberDevices+1)*sizeof(void*));
    unsigned char* TmpTypes = (unsigned char*) Kmalloc::Malloc((NumberDevices+1));
    if(TmpDevices == 0 || TmpTypes == 0){ Console::PrintString("Cant Alloc Memory For Devices\n"); asm("cli\nhlt"); }
    if(Devices != 0)
    {
        for(int i=0; i< NumberDevices; i++)
        {
            TmpDevices[i] = Devices[i];
            TmpTypes[i] = DeviceTypes[i];
        }
        Kmalloc::Free(DeviceTypes);
        Kmalloc::Free(Devices);
    }
    else
    {
        Console::PrintString("Allocing First Area For Devices\n");
    }
    TmpTypes[NumberDevices] = DeviceType;
    TmpDevices[NumberDevices] = HeaderStart;

    Devices = TmpDevices;
    DeviceTypes = TmpTypes;
    NumberDevices++;
}


void Madt::DefineLAPIC(void* HeaderStart)
{
    Console::PrintString("Found LAPIC(");
    struct LAPIC* LApic = (struct LAPIC*)(HeaderStart+2);
    Console::PrintInt((unsigned int)LApic->AcpiID, 0);
    Console::PrintString(",");
    Console::PrintInt((unsigned int)LApic->ApicID, 0);
    Console::PrintString("),");


}

void Madt::DefineDevice(unsigned char EntryType, void* HeaderStart)
{

    switch(EntryType)
    {
        case 0:
            DefineLAPIC(HeaderStart);
            break;
        case 1:
            Console::PrintString("Found I/O APIC,");
            break;
        case 2:
            Console::PrintString("Found I/O APIC Int OVER,");
            break;
        case 3:
            Console::PrintString("Found I/O APIC NMI,");
            break;
        case 4:
            Console::PrintString("Found LAPIC NMI,");
            break;
        case 5:
            Console::PrintString("Found LAPIC ADDROVER,");
            break;
        default:
            Console::PrintString("UnknownDev:");
            Console::PrintInt((unsigned int)EntryType, 0);
            Console::PrintString(",");
            break;
    }

    AddToDeviceList(EntryType, (HeaderStart+2));
}

void Madt::DisablePIC()
{

    IO::outb(0x20, 0x11);
    IO::iowait();
    IO::outb(0xA0, 0x11);
    
    IO::iowait();
    IO::outb(0x21, 0x20);
    IO::iowait();
    IO::outb(0xA1, 0x28);

    IO::iowait();
    IO::outb(0x21, 4);
    IO::iowait();
    IO::outb(0xA1, 2);

    IO::iowait();
    IO::outb(0x21, 1);
    IO::iowait();
    IO::outb(0xA1, 1);

    // Now that we enabled the PIC we now just mask all its ints

    IO::outb(0x21, 0xff);
    IO::iowait();
    IO::outb(0xA1, 0xff);

}

void Madt::ScanMadt(void* MadtTable)
{
    Devices = (void**)0;
    DeviceTypes = (unsigned char*)0;
    NumberDevices = 0;

    struct ACPISDTHeader* Madt = (struct ACPISDTHeader*) MadtTable;

    if(!Apic::CheckSum(MadtTable, Madt->Length))
    {
        Console::PrintString("Backup Checksum For MADT failed\n");
        asm("cli\nhlt");
    }
    Console::PrintString("Ran Finnal Checksum, Scaning Though Madt Tables!\n");

    unsigned int* LocalApicAddrPTR = (unsigned int*)((unsigned int)MadtTable+sizeof(struct ACPISDTHeader));
    unsigned int LocalApicAddr = LocalApicAddrPTR[0];
    unsigned int Flags = LocalApicAddrPTR[1]; // dumbist way todo it but it should work lol
    if(Flags & 1)
    {
        Console::PrintString("Dual APIC/PIC Found, reseting...");

        DisablePIC();
        Console::PrintString("Done\n");
    }
    

    unsigned int CurrentOffset = 0x2c;
    while(Madt->Length > CurrentOffset)
    {
        unsigned char EntryType = ((unsigned char*)(CurrentOffset+(unsigned int)MadtTable))[0];
        unsigned char EntrySize = ((unsigned char*)(CurrentOffset+(unsigned int)MadtTable))[1];
        DefineDevice(EntryType, ((void*)(CurrentOffset+(unsigned int)MadtTable)));
        CurrentOffset += EntrySize;
    }
    Console::PrintString("\n");

    if(Madt::LAPICREGAddr != (void*)LocalApicAddr)
    {
        Console::PrintString("Warning Conflict with MSR and Madt address\nReseting MSR!...");
        unsigned int MsrL;
        unsigned int MsrH;

        
        GetMSR(0x1B, &MsrL, &MsrH);

        MsrH = 0;
        Console::PrintString("Changing:");
        Console::PrintInt(MsrL, 1);
        Console::PrintString(" into ");
        Console::PrintInt((unsigned int)LocalApicAddr, 1);

        MsrL = (unsigned int)LocalApicAddr;

        SetMSR(0x1b, MsrL, MsrH);
        Console::PrintString("...Done\n");
        Madt::LAPICREGAddr = (void*)LocalApicAddr;
        
    }


    struct MallocOverWrite TmpTable;

    TmpTable.MemoryAddress = (unsigned int)Madt::LAPICREGAddr;
    TmpTable.NumberBytes = 0x100000;

    Kmalloc::AddOverwrite(TmpTable);

    Console::PrintString("LAPIC Reg Address:");
    Console::PrintInt((unsigned int)Madt::LAPICREGAddr, 1);
    Console::PrintString("\n");

}
