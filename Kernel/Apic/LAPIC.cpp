#include "../console/console.hpp"
#include "Madt.hpp"
#include "Apic.hpp"
#include "../malloc/malloc.hpp"
#include "../io.hpp"
#include "../Cpuid.hpp"
#include "../idt.hpp"
#include "../Paging/Paging.hpp"
extern "C" void CoreStartEntry();
extern "C" unsigned int CoreStartEntrySize;

unsigned int CurrentCoreStarting;
char* CoreStateList;
char** CoreStackPtrs; 
extern "C" unsigned int ReadEFlags();
extern unsigned int EEFlagsOverwride;


unsigned int* CoreCurrentThreadID;


unsigned int NumberCores;
unsigned char* CoreIDS;

void Apic::WriteLAPICReg(unsigned int Offset, unsigned int Data)
{
    
    if(Paging::PagingSetup){
            asm("cli");
            unsigned int Cr0;
            asm volatile("mov %%cr0, %0" : "=a"(Cr0));
            Cr0 &= ~(0x80000001);
            asm volatile("mov %0, %%cr0" :: "a"(Cr0));
        }
    
    asm("cli");
    unsigned char IsX2Apic = 0;
    unsigned int Unused;
    unsigned int MsrL;
    GetMSR(0x1b, &MsrL, &Unused);
    if(MsrL & (1 << 10))
    {
        IsX2Apic = 1;
    }
    
    if(!IsX2Apic){

        


        unsigned int volatile* Addr = (unsigned int volatile*)( ((unsigned int)Madt::LAPICREGAddr) + Offset);
        *Addr = Data;

        
    }
    else
    {
        unsigned int Registor = 0x800 + (Offset / 0x10);
        SetMSR(Registor, 0, 0);
    }

    if(Paging::PagingSetup)
        {
            unsigned int Cr0;
            asm volatile("mov %%cr0, %0" : "=a"(Cr0));
            Cr0 |= (0x80000001);
            asm volatile("mov %0, %%cr0" :: "a"(Cr0));
        }

}
unsigned int Apic::ReadLAPICReg(unsigned int Offset)
{
   if(Paging::PagingSetup){
            asm("cli");
            unsigned int Cr0;
            asm volatile("mov %%cr0, %0" : "=a"(Cr0));
            Cr0 &= ~(0x80000001);
            asm volatile("mov %0, %%cr0" :: "a"(Cr0));
        }
    
    
    asm("cli");
    unsigned char IsX2Apic = 0;
    unsigned int Unused;
    unsigned int MsrL;
    GetMSR(0x1b, &MsrL, &Unused);
    if(MsrL & (1 << 10))
    {
        IsX2Apic = 1;
    }

    if(!IsX2Apic){
        unsigned int Addr = ((unsigned int)Madt::LAPICREGAddr) + Offset;
        unsigned int Output = 0;

        


        asm volatile("movl (%1), %0": "=r"(Output) : "r"(Addr));


        if(Paging::PagingSetup)
        {
            unsigned int Cr0;
            asm volatile("mov %%cr0, %0" : "=a"(Cr0));
            Cr0 |= (0x80000001);
            asm volatile("mov %0, %%cr0" :: "a"(Cr0));
        }

        return Output;
    }
    else
    {
        unsigned int Registor = 0x800 + (Offset / 0x10);
        unsigned int MsrL, MsrH;
        GetMSR(Registor, &MsrL, &MsrH);
        if(Paging::PagingSetup)
        {
            unsigned int Cr0;
            asm volatile("mov %%cr0, %0" : "=a"(Cr0));
            Cr0 |= (0x80000001);
            asm volatile("mov %0, %%cr0" :: "a"(Cr0));
        }
        return MsrL;
    }
} 
void Apic::StartLAPICS()
{
    if(Madt::NumberDevices == 0)
    {
        Console::PrintString("MADT HASNT BEEN PARSED YET\n");
        asm("cli\nhlt");
    }


    // slow but im not sure a better way todo this yet
    // TODO make faster (if possible)
    // poriorty is lowist though
    unsigned int NumberLAPICs= 0;
    for(int i=0; i< Madt::NumberDevices; i++)
    {
        if(Madt::DeviceTypes[i] == 0 /*LAPIC Device Code*/)
        {
            NumberLAPICs++;
        }
    }

    struct LAPIC** LApicArray = (struct LAPIC**)Kmalloc::Malloc(sizeof(void*)*NumberLAPICs);
    if(LApicArray == 0)
    {
        Console::PrintString("Cant Alloc Memory For LAPICS\n");
        asm("cli\nhlt");
    }

    unsigned int NumberLAPICSAssined = 0;
    for(int i=0; i< Madt::NumberDevices; i++ )
    {
        if(Madt::DeviceTypes[i] == 0)
        {
            LApicArray[NumberLAPICSAssined] = (struct LAPIC*)Madt::Devices[i];
            NumberLAPICSAssined++;
        }
    }

    Console::PrintString("Confermed Existence of ");
    Console::PrintInt(NumberLAPICSAssined, 0);
    Console::PrintString(" Cores\n");

    Console::PrintString("Checking LAPIC States\n");

    unsigned char* LApicStates = (unsigned char*)Kmalloc::Malloc(sizeof(unsigned char*)*NumberLAPICs);
    unsigned int NumberCoresActive = 0;
    for(int i=0; i< NumberLAPICs; i++)
    {
        if(LApicArray[i]->Flags & 1)
        {
            LApicStates[i] = 1;
        }
        else if(LApicArray[i]->Flags & 2)
        {
            LApicStates[i] = 2;
        }
        else
        {
            LApicStates[i] = 0;
            NumberCoresActive--;
        }
        NumberCoresActive++;
    }
    Console::PrintInt(NumberCoresActive, 0);
    Console::PrintString(" Number Cores Enablable!\n");
    Console::PrintString("Enabling LAPIC Registors\n");

    Console::PrintString("Local Core ID:");
    unsigned int BscID = ReadLAPICReg(0x20);
    Console::PrintInt(BscID, 0);
    WriteLAPICReg(0xb0, 0);
    WriteLAPICReg(0xf0, 0x00010000);
    Console::PrintString("\n");
    
    Console::PrintString("Started LAPIC ints, Starting Cores\n");
    // copy the corestartfunc to a clear spot in memory
    //
    //for now we will just use 0x8000
     
    for(int i=0; i< CoreStartEntrySize; i++)
    {
        char* CoreStartPoint = (char*) 0x8000;

        CoreStartPoint[i] = ((char*)CoreStartEntry)[i];
    }

    Console::PrintString("Core Entry Point Ready!\n");


    CoreStateList = (char*) Kmalloc::Malloc(NumberLAPICs); 
    CoreStackPtrs = (char**) Kmalloc::Malloc(NumberLAPICs*sizeof(char**));
    if(CoreStateList == 0 || CoreStackPtrs == 0){
        Console::PrintString("Cant Alloc Memory For Cores\n");
        asm("cli\nhlt");
    }

    for(int i=0; i< NumberLAPICs; i++)
    {
        CoreStateList[i] = 0;
        CoreStackPtrs[i] = (char*) Kmalloc::Malloc(4*0x1000);
        
        
        
        
        if(CoreStackPtrs[i] == 0)
        {
            Console::PrintString("ERROR Cant Alloc Memory for core stack!\n");
            asm("cli\nhlt");
        }
        
        for(int Clearbyte = 0; Clearbyte < 4*0x1000; Clearbyte++)
        {
            CoreStackPtrs[i][Clearbyte] = 0;
        }
        CoreStackPtrs[i] += 4*0x1000;
    }
    
     
    unsigned char IsX2Apic = 0;
    unsigned int Unused;
    unsigned int MsrL;
    GetMSR(0x1b, &MsrL, &Unused);
    if(MsrL & (1 << 10))
    {
        IsX2Apic = 1;
    }
    if(IsX2Apic)
    {
        Console::PrintString("Using 2XApic\n");
    }


    Console::PrintString("Allocated Memory For Core Stacks\n");
    NumberCores= 0;
    for(int i=0; i< NumberLAPICs; i++)
    {
        if(LApicArray[i]->ApicID == BscID){ Console::PrintString("Skiping BSC\n") ;continue; }
        
        CurrentCoreStarting = i;
        asm("cli");
        if(LApicStates[i] != 0)
        {
            
            Console::PrintString("Starting Core ID:");
            Console::PrintInt(((unsigned int)LApicArray[i]->ApicID), 0);
            Console::PrintString("...");
            



            if(!IsX2Apic){
                WriteLAPICReg(0x310, ((unsigned int)LApicArray[i]->ApicID << 24)); // have been combined
                WriteLAPICReg(0x300, (0b101 << 8));

                while(ReadLAPICReg(0x300) & (1 << 12));
            }
            else
            {
                unsigned int Registor = 0x800 + (0x300 / 0x10);
                SetMSR(Registor, (0b101 << 8), (unsigned int)LApicArray[i]->ApicID);
            }

            
            

            if(!IsX2Apic){
                WriteLAPICReg(0x310,  ((unsigned int)LApicArray[i]->ApicID << 24));
                WriteLAPICReg(0x300, (0b101 << 8) | (1 << 14));

                while(ReadLAPICReg(0x300) & (1 << 12));
            }
            else
            {
                unsigned int Registor = 0x800 + (0x300 / 0x10);
                SetMSR(Registor, (0b101 << 8), (unsigned int)LApicArray[i]->ApicID);
            }

            


            if(!IsX2Apic){
                WriteLAPICReg(0x310,  ((unsigned int)LApicArray[i]->ApicID << 24));
                WriteLAPICReg(0x300, (0b110 << 8) | 0x8);
                IO::iowait();
                WriteLAPICReg(0x300, (0b110 << 8) | 0x8);
            }
            else
            {
                
                unsigned int Registor = 0x800 + (0x300 / 0x10);
                SetMSR(Registor, (0b110 << 8) | 0x08, (unsigned int)LApicArray[i]->ApicID);
                IO::iowait();
                SetMSR(Registor, (0b110 << 8) | 0x08, (unsigned int)LApicArray[i]->ApicID);
            }
           
            while(CoreStateList[i] == 0);
        
            
            Console::PrintString("Done\n");
        }
        NumberCores++;

    }
    Console::PrintString("Storeing LAPIC Ids for later\n");
    NumberCores++; // counting for BSD
    CoreIDS = (unsigned char*) Kmalloc::Malloc(NumberCores);
    CoreCurrentThreadID = (unsigned int*) Kmalloc::Malloc(NumberCores*sizeof(unsigned int));
    unsigned int CoreIndex = 0;
    for(int i=0; i< NumberLAPICs; i++)
    { 
        if(LApicStates[i] == 0){ continue; }
        CoreIDS[CoreIndex] = LApicArray[i]->ApicID;
        CoreCurrentThreadID[i] = 0;
        CoreIndex++;
    }
    


    //Kmalloc::Free(CoreStackPtrs);
    //Kmalloc::Free(LApicStates);
    Kmalloc::Free(LApicArray);

    Console::PrintString("Cores Online, Setting up I/O APIC!\n");
}

unsigned char PITOnline = 0;



void CpuIntOverload(unsigned int ISR)
{
    if(ISR == 0x32){
        Apic::WriteLAPICReg(0xf0, 0xFF); // stop timer

        // here we will attempt to half the speed of the LAPIC TIMER
        
        Apic::WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
        Apic::WriteLAPICReg(0x3E0, 0x3);
        Apic::WriteLAPICReg(0x380,(Apic::ReadLAPICReg(0x380)*1.3));
        Apic::WriteLAPICReg(0xf0, 0x100);
    }
}

void CpuUnderLoad()
{
    Apic::WriteLAPICReg(0xf0, 0xFF); // stop timer

    // here we will attempt to double the speed of the LAPIC TIMER
        
    Apic::WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
    Apic::WriteLAPICReg(0x3E0, 0x3);
    Apic::WriteLAPICReg(0x380,(Apic::ReadLAPICReg(0x380)/1.1));
    Apic::WriteLAPICReg(0xf0, 0x100);

}


extern "C" void AfterDelay(unsigned int Tmp)
{
    //Apic::WriteLAPICReg(0xb0, 0);
}



unsigned int Apic::NumberCyclesPerMs;
void Apic::StartLApicTimer(unsigned int NumberTimesPerMS)
{
    asm("cli");
    Console::PrintString("Starting APIC Timer...");
    WriteLAPICReg(0x3E0, 0x3);
    WriteLAPICReg(0x380, 0xFFFFFFFF);

    for(int i=0; i< 0xFFFFF; i++);
    NumberCyclesPerMs = (0xFFFFFFFF - ReadLAPICReg(0x390));

    IDT::SetISR(0x32, AfterDelay, 1);
    IDT::CpuIntStormHandler = CpuIntOverload;
    IDT::CpuUnderFlowHandler = CpuUnderLoad;

    WriteLAPICReg(0xf0, 0x100);
    WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
    WriteLAPICReg(0x3E0, 0x3);
    WriteLAPICReg(0x380, 0xFFFFF);
    Console::PrintString("Starting Each Core's APIC Timer...\n");
    Paging::PageSpinLock = 0;

    for(int i=1; i< NumberCores; i++)
    {
        CoreStateList[i] = 2;

        while(CoreStateList[i] != 3);
    }
    for(int i=1; i< NumberCores; i++)
    {
        CoreStateList[i] = 4;
        while (CoreStateList[i] != 5);
        Console::PrintString("Core Confermed Online\n");
        
    }
    asm("sti");
}