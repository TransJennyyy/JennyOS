#include "Apic.hpp"
#include "../console/console.hpp"
#include "../malloc/malloc.hpp"
#include "Madt.hpp"
unsigned int* Apic::EntryPtrs;
unsigned int Apic::NumberRSDTEntrys;
const char* Apic::NeededDeviceSigs[NumberDevicesNeeded] = {"APIC"};

void* Apic::FindRSDTEntryWithSig(char Sig[4])
{
    for(int i=0; i < NumberRSDTEntrys; i++)
    {
        struct ACPISDTHeader* Rsdrheader = (struct ACPISDTHeader*)EntryPtrs[i]; 
        unsigned char FoundMatch = 1;
        for(int j=0; j< 4; j++)
        {
            if(Sig[j] != Rsdrheader->Signature[j])
            {
                FoundMatch = 0;
                break;
            }
        }
        if(FoundMatch)
        {
            // we run a check sum here to make sure nothing is broken
            if(!CheckSum(Rsdrheader, Rsdrheader->Length))
            {
                Console::PrintString("Kernel Asked for ACPI device, that is broken\n");
                return 0 ;
            }


            return (void*)Rsdrheader;
        }
    }
    return 0;
}

void Apic::ScanRSDT()
{
    Console::PrintString("Starting Scan\n");
    void* RsdtPtr = (void*) ((struct RSDP_t*)RdspPtr)->RsdtAddress;
    if(Madt::AcpiVersionNumber > 1){ RsdtPtr = (void*) ((struct XSDP_t*)RdspPtr)->XsdtAddress; }

    
    
    
    struct ACPISDTHeader* RSDT = (struct ACPISDTHeader*)RsdtPtr;
    if(!CheckSum(RSDT,  RSDT->Length))
    {
        Console::PrintString("check sum failed of RSDT\n");
        asm("cli\nhlt");
    }


    struct MallocOverWrite TmpTable;
    TmpTable.MemoryAddress = (unsigned int)RsdtPtr;
    TmpTable.NumberBytes = RSDT->Length;
    //Kmalloc::AddOverwrite(TmpTable);

    unsigned int* RsdtArrayStart = (unsigned int*)  ((unsigned int)RsdtPtr+sizeof(ACPISDTHeader));
    unsigned int NumberEntrys = (RSDT->Length - sizeof(ACPISDTHeader));
    NumberEntrys = (Madt::AcpiVersionNumber <= 1)?NumberEntrys/4:NumberEntrys/8;
    Console::PrintString("Found ");
    Console::PrintInt(NumberEntrys, 0);
    Console::PrintString(" Number RSDT Entrys\n");
    NumberRSDTEntrys = NumberEntrys;
    


    EntryPtrs = (unsigned int*)Kmalloc::Malloc(NumberEntrys*sizeof(unsigned int));
    if(EntryPtrs == 0){ Console::PrintString("Cant Alloc Memory For ACPI tables"); asm("cli\nhlt"); }
    if(Madt::AcpiVersionNumber <= 1){
        for(int i=0; i< NumberEntrys; i++)
        {
            struct ACPISDTHeader* RsdtEntry = (struct ACPISDTHeader*) RsdtArrayStart[i];
            TmpTable.MemoryAddress = (unsigned int)RsdtEntry;
            TmpTable.NumberBytes = RsdtEntry->Length;
            //Kmalloc::AddOverwrite(TmpTable);
            
            EntryPtrs[i] = RsdtArrayStart[i];
        }
    }
    else
    {
        unsigned int CurrentEntryIndex = 0;
        unsigned int* XsdtArray = RsdtArrayStart;
        for(int i=0; i< NumberEntrys*2; i+=2)
        {
            EntryPtrs[CurrentEntryIndex] = XsdtArray[0];
            CurrentEntryIndex++;
            XsdtArray=  (unsigned int*) (((unsigned int)XsdtArray) + 8);
        }
    }

    for(int i=0; i< NumberDevicesNeeded; i++)
    {
        void* Dev = FindRSDTEntryWithSig((char*)NeededDeviceSigs[i]);
        if(Dev == 0)
        {
            Console::PrintString("Kernel Needed Device ");
            Console::PrintString(NeededDeviceSigs[i]);
            Console::PrintString(" Doesnt Exist\n");
            asm("hlt");
        }
    }
    Console::PrintString("All Devices Needed Are Online!\nKernel Is Ready For Boot\n");
     

}
