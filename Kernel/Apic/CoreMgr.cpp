#include "Madt.hpp"
#include "Apic.hpp"
#include "../console/console.hpp"
#include "../Cpuid.hpp"
#include "../io.hpp"
#include "../Paging/Paging.hpp"

extern char* CoreStateList;
extern unsigned char* CoreIDS;
extern unsigned int CurrentCoreStarting;

extern unsigned int* NumberIntsPerCore;
extern unsigned int NumberCores;

void CoreEntry(unsigned int CoreIndex)
{ 
    unsigned char SupportsX2Apic = 0;

    unsigned int edx, unused;
    Cpuid(1, &unused, &unused, &unused, &edx);

    if(edx & (1 << 21))
    {
        SupportsX2Apic=1;
    }
    
    unsigned int MsrL = 0;
    unsigned int MsrH = 0;
    GetMSR(0x1b, &MsrL, &MsrH);

    if(!(MsrL & (1 << 10)) && SupportsX2Apic)
    {
        MsrL |= (1 << 10);
    }
    
    SetMSR(0x1b, MsrL, MsrH);
    Apic::WriteLAPICReg(0xf0, 0xff | 0x100);
    asm("sti");
    CoreStateList[CoreIndex] = 1;
                                       
    while(CoreStateList[CoreIndex] != 2);
    Paging::Enable();
    Console::PrintString("Core Starting Timer...\n");

    asm("cli");
    
    CoreStateList[CoreIndex] = 3;
    while(CoreStateList[CoreIndex] != 4);
    Apic::WriteLAPICReg(0xf0, 0x100);
    Apic::WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
    Apic::WriteLAPICReg(0x3E0, 0x3);
    Apic::WriteLAPICReg(0x380, 0xFFFFF);
    asm("sti");

    while(1)
    {
        //while(CoreStateList[CoreIndex] == 4);
        //Apic::WriteLAPICReg(0xb0, 0); // ensures that when it ends up back here that the timer resets at some point
        //Apic::WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
        //Apic::WriteLAPICReg(0x3E0, 0x3);
        //Apic::WriteLAPICReg(0x380,(Apic::ReadLAPICReg(0x380)));
        //Apic::WriteLAPICReg(0xf0, 0x100); // reset the core timer
        asm("sti");
    }


    
    

    
 }





extern "C" void CoreCEntry()
{
    unsigned int CoreIndex = CurrentCoreStarting;
   
    CoreEntry(CoreIndex);
}
