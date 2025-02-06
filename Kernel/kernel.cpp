
void CKernelEntry();
extern "C" void KernelEntry()
{
    CKernelEntry();
}


#include "console/console.hpp"
#include "malloc/malloc.hpp"
#include "idt.hpp"
#include "Apic/Apic.hpp"
#include "Apic/Madt.hpp"
#include "PCI/PCI.hpp"
#include "Paging/Paging.hpp"
#include "FS/FileSystem.hpp"
#include "Exec/Thread.hpp"
#include "Syscall/Syscall.hpp"
#include "Ps2/Ps2.hpp"
#include "KernelRamDiskApps/CommandLine/CommandLine.hpp"
#include "KernelRamDiskApps/RamDiskMgr.hpp"
extern unsigned int NumberCores;
extern unsigned int NumberThreads;
extern Thread* ThreadArray;

extern "C" void StartThread(unsigned int AppEntry, unsigned int AppStack);

void CKernelEntry()
{
    NumberCores = 0;
    ThreadArray = 0;
    NumberThreads = 0;
    Paging::PagingSetup = 0;
    Apic::NumberCyclesPerMs = 0xFFFFF; // leave it at this number on start, changes later
    Console::VGAInit();
    Console::PrintString("Jenny Os Is Starting...\n");
    Kmalloc::MallocInit();

    //asm("cli\nhlt");

    IDT::InitIDT();

    Apic::init();
    Madt::ScanMadt(Apic::FindRSDTEntryWithSig((char*)"APIC"));
    Apic::StartLAPICS();

    //asm("cli\nhlt");

    // TODO FIX IOAPIC (AND ADD MSI and MSI-X)
    //Apic::StartIOAPICS();

    PCI::FindDevices();

    //Paging::Init();
    Apic::StartLApicTimer(10);

    FS::init();
    RamDiskMgr::init(); // start the override mgr

    IDT::SetISR(0x80, 0, 0);

    SysCalls::Init();
    Ps2::Init();

    Thread::SpinLock = 0;

    void* CommandLineAddress = RamDiskMgr::ReadFile("/TestApp.jexe");
    if(CommandLineAddress == 0)
    {
        Console::PrintString("Cant Get Start Up App\n");
        while(1);
    }

    Thread::InitThread(CommandLineAddress, 0, 1);
    Apic::WriteLAPICReg(0xb0, 0);
    Apic::WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
    Apic::WriteLAPICReg(0x3E0, 0x3);
    Apic::WriteLAPICReg(0x380,(Apic::ReadLAPICReg(0x380)));
    Apic::WriteLAPICReg(0xf0, 0x100); // reset the core timer
    asm("sti");
    while(1);


}
