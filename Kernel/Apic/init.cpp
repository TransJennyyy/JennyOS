#include "Apic.hpp"
#include "../console/console.hpp"
#include "../Cpuid.hpp"
#include "Madt.hpp"
unsigned char Apic::HasMSR;

void* Apic::RdspPtr;

unsigned char Apic::CheckSum(void* Addr, unsigned int SizeBytes)
{
    char* AddrChar = (char*) Addr;
    unsigned char Sum = 0;
    for(int i=0; i< SizeBytes; i++)
    {
        Sum += AddrChar[i];
    }
    return Sum == 0;
}

void Apic::FindRDSP()
{
    const char* CmpString = "RSD PTR ";
    for(unsigned int i=0xE0000; i < 0xFFFFF; i++)
    {
        char* TmpPtr = ((char*)i);
        unsigned char FoundRSDP = 1;
        for(int letintdex = 0; letintdex < 8; letintdex++)
        {
            if(TmpPtr[letintdex] != CmpString[letintdex]){ FoundRSDP = 0; break; }
        }
        if(FoundRSDP)
        { 
            RdspPtr = (void*)TmpPtr; 
            break;
        }
    }
}

unsigned int Madt::AcpiVersionNumber;
void Apic::init()
{
    Console::PrintString("Checking if cpu has MSRs\n");
    unsigned int unused = 0;
    unsigned int edx = 0; 
    unsigned int ecx = 0;
    Cpuid(1, &unused, &unused, &unused, &edx);
    
    HasMSR = 0;

    if(edx & (1 << 5) != 0)
    {
        HasMSR = 1;
        unsigned int MsrL = 0;
        unsigned int MsrH = 0;
        GetMSR(0x1B, &MsrL, &MsrH);
        Console::PrintInt(MsrL, 1);
        Console::PrintString(",");
        MsrL |= (1 << 11);
        Console::PrintInt(MsrL, 1);
        SetMSR(0x1B, MsrL, MsrH);
        Console::PrintString("\nCpu Has MSR\nEnsured Apic Online!\n");
        Madt::LAPICREGAddr = (void*)(MsrL & 0xfffff0000);
    }
    if(edx & (1 << 21))
    {
        Console::PrintString("x2apic enabling...");
        unsigned int Msrl;
        unsigned int MsrH;
        GetMSR(0x1b, &Msrl, &MsrH);

        Msrl |= (1 << 10); // enable it

        SetMSR(0x1b, Msrl, MsrH);
        Console::PrintString("Done\n");
    }
   
    RdspPtr = (void*)0;
    FindRDSP();
    Console::PrintString("RSDP found at address:");
    Console::PrintInt((unsigned int)RdspPtr, 1);
    Console::PrintString("\n");
    if(RdspPtr == 0){ Console::PrintString("ERROR Cant Find RSDP\n"); asm volatile("cli\nhlt");  }
    struct RSDP_t* Rsdp = (struct RSDP_t*)RdspPtr;
    Madt::AcpiVersionNumber = Rsdp->Revision;
    if(Rsdp->Revision > 2){ Console::PrintString("unknown ACPI version!\n"); asm("cli\nhlt"); } 
    if(Rsdp->Revision != 0){ Console::PrintString("XDST table found!\n");}
    if(!CheckSum(Rsdp, sizeof(struct RSDP_t))){ Console::PrintString("Error: Checksum fail of RSDP\n"); asm("cli\nhlt"); }
    Console::PrintString("Got RSDP Address and Ran Checksum | Starting Scan Of RSDT\n");

    ScanRSDT();
}



