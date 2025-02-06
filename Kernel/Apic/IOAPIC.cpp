#include "Apic.hpp"
#include "Madt.hpp"
#include "../console/console.hpp"
#include "../malloc/malloc.hpp"
#include "../idt.hpp"
#include "../io.hpp"
#include "../Cpuid.hpp"
void TmpFunc(unsigned int ISR);

extern unsigned int NumberCores;
extern unsigned char* CoreIDS;






void Apic::WriteIOAPIC(void* IoApicBase, unsigned int Data, unsigned int Offset)
{

    unsigned int volatile* IoApicAddr = (unsigned int volatile*)IoApicBase;
    unsigned int volatile* IoApicReg = (unsigned int volatile*) ((unsigned int)IoApicBase + 0x10);
    *IoApicAddr = Offset;
    *IoApicReg = Data;
}

unsigned int Apic::ReadIOAPIC(void* IoApicBase, unsigned int Offset)
{
    unsigned int volatile* IoApicAddr = (unsigned int volatile*)IoApicBase;
    unsigned int volatile* IoApicReg = (unsigned int volatile*) ((unsigned int)IoApicBase + 0x10);
    *IoApicAddr = Offset;
    return *IoApicReg;
}
struct IOAPICINTOVER** IOAPICIntOverwrites; 
struct IOAPIC** IOApics;
unsigned int NumberIOAPIC;
unsigned int NumberIOAPICIntOver;
unsigned short* VectorOfGsi;
// first byte is vector
// next byte is ApicID of the core
// eg(0 for BSD(most the time), 1 for AP1 etc )

unsigned int Apic::ConvertIRQToGSI(unsigned char IRQ, unsigned char* WasFound, unsigned int* DeviceIndex)
{
    *WasFound = 1;
    for(int i=0; i< NumberIOAPICIntOver; i++)
    {
        if(IOAPICIntOverwrites[i]->IRQ == IRQ) { *DeviceIndex=i; ;return IOAPICIntOverwrites[i]->GlobalSystemInt; }
    }
    *WasFound = 0;
    *DeviceIndex=0;
    return 0;
}




void Apic::UnmaskIRQ(unsigned int GSI)
{
    if(VectorOfGsi == 0)
    {
        Console::PrintString("Invaild Translasion table Error\n");
        return;
    }
    unsigned int IoApicIndex = 0;
    for(int i=0; i< NumberIOAPIC; i++)
    {
        if(IOApics[i]->GlobalSystemBase < GSI)
        {
            unsigned int MaxNumberEntrys = ReadIOAPIC((void*)IOApics[i]->ApicAddr, 1) >> 16;
            if(IOApics[i]->GlobalSystemBase - GSI < MaxNumberEntrys) { IoApicIndex = i; break; }
        }    
    }
    unsigned int DeviceIndex = 0;
    for(int i=0; i < NumberIOAPICIntOver; i++)
    {
        if(IOAPICIntOverwrites[i]->GlobalSystemInt == GSI){ DeviceIndex = i; break; }
    }
    unsigned int OffsetGSI = IOAPICIntOverwrites[DeviceIndex]->GlobalSystemInt - IOApics[IoApicIndex]->GlobalSystemBase;
    Console::PrintString("unmasking GSI:");
    Console::PrintInt(GSI, 1);
    Console::PrintString(",IO:");
    Console::PrintInt(DeviceIndex,0);
    unsigned int Reg = 0x10 + (OffsetGSI * 2);
    Console::PrintString(",Reg:");
    Console::PrintInt(Reg, 1);
    Console::PrintString("\n");
    unsigned int TableLow = ReadIOAPIC((void*)IOApics[IoApicIndex]->ApicAddr, Reg);
    unsigned int TableHigh = ReadIOAPIC((void*)IOApics[IoApicIndex]->ApicAddr, Reg+1);
    if( ((TableLow & (1 << 16)) == 0))
    {
        Console::PrintString("Warning Already unmasked\n");
    }
    TableLow &= ~(1 << 16);

    WriteIOAPIC((void*)IOApics[IoApicIndex]->ApicAddr, TableLow, Reg);
    WriteIOAPIC((void*)IOApics[IoApicIndex]->ApicAddr,TableHigh, Reg+1);
}


void Apic::MaskIRQ(unsigned int GSI)
{
    if(VectorOfGsi == 0)
    {
        Console::PrintString("Invaild Translasion table Error\n");
        return;
    }
    unsigned int IoApicIndex = 0;
    for(int i=0; i< NumberIOAPIC; i++)
    {
        if(IOApics[i]->GlobalSystemBase < GSI)
        {
            unsigned int MaxNumberEntrys = ReadIOAPIC((void*)IOApics[i]->ApicAddr, 1) >> 16;
            if(IOApics[i]->GlobalSystemBase - GSI < MaxNumberEntrys) { IoApicIndex = i; break; }
        }    
    }
    unsigned int DeviceIndex = 0;
    for(int i=0; i < NumberIOAPICIntOver; i++)
    {
        if(IOAPICIntOverwrites[i]->GlobalSystemInt == GSI){ DeviceIndex = i; break; }
    }
    unsigned int OffsetGSI = IOAPICIntOverwrites[DeviceIndex]->GlobalSystemInt - IOApics[IoApicIndex]->GlobalSystemBase;
    Console::PrintString("Masking GSI:");
    Console::PrintInt(GSI, 1);
    Console::PrintString(",IO:");
    Console::PrintInt(DeviceIndex,0);
    unsigned int Reg = 0x10 + (OffsetGSI * 2);
    Console::PrintString(",Reg:");
    Console::PrintInt(Reg, 1);
    Console::PrintString("\n");
    unsigned int TableLow = ReadIOAPIC((void*)IOApics[IoApicIndex]->ApicAddr, Reg);
    unsigned int TableHigh = ReadIOAPIC((void*)IOApics[IoApicIndex]->ApicAddr, Reg+1);
    if( ((TableLow & (1 << 16)) != 0))
    {
        Console::PrintString("Warning Already masked\n");
    }
    TableLow |= (1 << 16);

    WriteIOAPIC((void*)IOApics[IoApicIndex]->ApicAddr, TableLow, Reg);
    WriteIOAPIC((void*)IOApics[IoApicIndex]->ApicAddr,TableHigh, Reg+1);
}







void Apic::StartIOAPICS()
{
    Console::PrintString("IOAPIC System init...\n");



    NumberIOAPIC = 0;
    NumberIOAPICIntOver =0;
    unsigned int NumberIOAPICNMI = 0;
    for(int i=0; i< Madt::NumberDevices; i++)
    {
        if(Madt::DeviceTypes[i] == 1 /* IOAPIC Device Code */)
        {
            NumberIOAPIC++; 
        }
        if(Madt::DeviceTypes[i] == 2 /* IOAPIC Int Source*/)
        {
            NumberIOAPICIntOver++;
        }
        if(Madt::DeviceTypes[i] == 3)
        {
            NumberIOAPICNMI++;
        }
    }

    Console::PrintString("Number IOApic:");
    Console::PrintInt(NumberIOAPIC, 0);
    Console::PrintString("\n");

    Console::PrintString("Number IOApicINTOVER:");
    Console::PrintInt(NumberIOAPICIntOver, 0);
    Console::PrintString("\n");
    
    Console::PrintString("Number IOApicNMI:");
    Console::PrintInt(NumberIOAPICNMI, 0);
    Console::PrintString("\n");

    // now we alloc memory for the IOAPIC and all its needed data

    IOApics = (struct IOAPIC**) Kmalloc::Malloc(NumberIOAPIC*sizeof(struct IOAPIC*));
    IOAPICIntOverwrites = (struct IOAPICINTOVER**) Kmalloc::Malloc(NumberIOAPICIntOver*sizeof(unsigned int));
    struct IOAPICNMI** IOAPICNMIs = (struct IOAPICNMI**) Kmalloc::Malloc(NumberIOAPIC*sizeof(unsigned int));


    unsigned int NumberIOAPICNMISDefined = 0;
    unsigned int NumberIOAPICSDefined = 0;
    unsigned int NumberIOAPICSIntOverwritesDefined = 0;
    for(int i=0; i< Madt::NumberDevices; i++)
    {
        
        if(Madt::DeviceTypes[i] == 1 /* IOAPIC Device Code */)
        {
            IOApics[NumberIOAPICSDefined] = (struct IOAPIC*)Madt::Devices[i];
            NumberIOAPICSDefined++;
        }
        if(Madt::DeviceTypes[i] == 2 /* IOAPIC Int Source*/)
        {
            IOAPICIntOverwrites[NumberIOAPICSIntOverwritesDefined] = (struct IOAPICINTOVER*)Madt::Devices[i];
            NumberIOAPICSIntOverwritesDefined++;
        }
        if(Madt::DeviceTypes[i] == 3 /* IOAPIC NMI*/)
        {
            IOAPICNMIs[NumberIOAPICNMISDefined] = (struct IOAPICNMI*)Madt::Devices[i];
            NumberIOAPICNMISDefined++;
        }
    }



    VectorOfGsi = (unsigned short*) Kmalloc::Malloc(NumberIOAPICIntOver*sizeof(unsigned short));
    if(VectorOfGsi == 0)
    {
            Console::PrintString("Error Allocing Memory for Conversion Table.");
            asm("cli");
            asm("hlt");
    }

    Console::PrintString("Setup IOAPIC!\n");
    asm("cli\nhlt");

    unsigned char WasFound;
    unsigned int DeviceIndex;
    unsigned int GlobalSystemIntPit = ConvertIRQToGSI(0, &WasFound, &DeviceIndex);
    if(!WasFound)
    {
        Console::PrintString("ERROR CANT FIND PIT\n");
        asm("cli");
        asm("hlt");
    }
    unsigned char PitVector = (VectorOfGsi[DeviceIndex] & 0xff);
    Console::PrintString("Pit GSI is:");
    Console::PrintInt(GlobalSystemIntPit, 1);
    Console::PrintString(",Vector:");
    Console::PrintInt((unsigned int)PitVector, 1);
    Console::PrintString("\n");
    Console::PrintString("Testing PIT...\n");
    UnmaskIRQ(GlobalSystemIntPit);
    IDT::SetISR(PitVector, TmpFunc, 1);  
    
    asm("sti");

    IO::outb(0x43, 0x30);
    unsigned short count = 1193182 / 1000;
    IO::outb(0x40, count & 0xff);
    IO::outb(0x40, (count >> 8) & 0xff);


}
        

void TmpFunc(unsigned int ISR)
{
    
    Console::PrintString("Got PIT! On Core:");
    Console::PrintInt((unsigned int)IDT::CoreIDPerCore[ISR], 0);
    Console::PrintString("\n");
    Apic::WriteLAPICReg(0xb0, 0);
}
