#include "idt.hpp"
#include "console/console.hpp"
#include "malloc/malloc.hpp"
#include "Apic/Apic.hpp"
#include "Apic/Madt.hpp"
#include "io.hpp"
#include "Paging/Paging.hpp"
#include "Exec/Thread.hpp"
#include "Ps2/Ps2.hpp"
#include "SpinLock.hpp"
#include "KernelRamDiskApps/RamDiskMgr.hpp"
unsigned char LastSeccond;
unsigned char CurrentCoreRunning = 0;
unsigned int NumberInts;
unsigned char StartedThread = 0;
extern unsigned char* FirstStartThread;
unsigned int StartingThread;
unsigned int Ps2SpinLock;


void TestSystemcall(unsigned int);
extern "C" void IDTENTRY0();
extern "C" void IDTENTRY1();
extern "C" void IDTENTRY2();
extern "C" void IDTENTRY3();
extern "C" void IDTENTRY4();
extern "C" void IDTENTRY5();
extern "C" void IDTENTRY6();
extern "C" void IDTENTRY7();
extern "C" void IDTENTRY8();
extern "C" void IDTENTRY9();
extern "C" void IDTENTRY10();
extern "C" void IDTENTRY11();
extern "C" void IDTENTRY12();
extern "C" void IDTENTRY13();
extern "C" void IDTENTRY14();
extern "C" void IDTENTRY15();
extern "C" void IDTENTRY16();
extern "C" void IDTENTRY17();
extern "C" void IDTENTRY18();
extern "C" void IDTENTRY19();
extern "C" void IDTENTRY20();
extern "C" void IDTENTRY21();
extern "C" void IDTENTRY22();
extern "C" void IDTENTRY23();
extern "C" void IDTENTRY24();
extern "C" void IDTENTRY25();
extern "C" void IDTENTRY26();
extern "C" void IDTENTRY27();
extern "C" void IDTENTRY28();
extern "C" void IDTENTRY29();
extern "C" void IDTENTRY30();
extern "C" void IDTENTRY31();
extern "C" void IDTENTRY32();
extern "C" void IDTENTRY33();
extern "C" void IDTENTRY34();
extern "C" void IDTENTRY35();
extern "C" void IDTENTRY36();
extern "C" void IDTENTRY37();
extern "C" void IDTENTRY38();
extern "C" void IDTENTRY39();
extern "C" void IDTENTRY40();
extern "C" void IDTENTRY41();
extern "C" void IDTENTRY42();
extern "C" void IDTENTRY43();
extern "C" void IDTENTRY44();
extern "C" void IDTENTRY45();
extern "C" void IDTENTRY46();
extern "C" void IDTENTRY47();
extern "C" void IDTENTRY48();
extern "C" void IDTENTRY49();
extern "C" void IDTENTRY50();
extern "C" void IDTENTRY51();
extern "C" void IDTENTRY52();
extern "C" void IDTENTRY53();
extern "C" void IDTENTRY54();
extern "C" void IDTENTRY55();
extern "C" void IDTENTRY56();
extern "C" void IDTENTRY57();
extern "C" void IDTENTRY58();
extern "C" void IDTENTRY59();
extern "C" void IDTENTRY60();
extern "C" void IDTENTRY61();
extern "C" void IDTENTRY62();
extern "C" void IDTENTRY63();
extern "C" void IDTENTRY64();
extern "C" void IDTENTRY65();
extern "C" void IDTENTRY66();
extern "C" void IDTENTRY67();
extern "C" void IDTENTRY68();
extern "C" void IDTENTRY69();
extern "C" void IDTENTRY70();
extern "C" void IDTENTRY71();
extern "C" void IDTENTRY72();
extern "C" void IDTENTRY73();
extern "C" void IDTENTRY74();
extern "C" void IDTENTRY75();
extern "C" void IDTENTRY76();
extern "C" void IDTENTRY77();
extern "C" void IDTENTRY78();
extern "C" void IDTENTRY79();
extern "C" void IDTENTRY80();
extern "C" void IDTENTRY81();
extern "C" void IDTENTRY82();
extern "C" void IDTENTRY83();
extern "C" void IDTENTRY84();
extern "C" void IDTENTRY85();
extern "C" void IDTENTRY86();
extern "C" void IDTENTRY87();
extern "C" void IDTENTRY88();
extern "C" void IDTENTRY89();
extern "C" void IDTENTRY90();
extern "C" void IDTENTRY91();
extern "C" void IDTENTRY92();
extern "C" void IDTENTRY93();
extern "C" void IDTENTRY94();
extern "C" void IDTENTRY95();
extern "C" void IDTENTRY96();
extern "C" void IDTENTRY97();
extern "C" void IDTENTRY98();
extern "C" void IDTENTRY99();
extern "C" void IDTENTRY100();
extern "C" void IDTENTRY101();
extern "C" void IDTENTRY102();
extern "C" void IDTENTRY103();
extern "C" void IDTENTRY104();
extern "C" void IDTENTRY105();
extern "C" void IDTENTRY106();
extern "C" void IDTENTRY107();
extern "C" void IDTENTRY108();
extern "C" void IDTENTRY109();
extern "C" void IDTENTRY110();
extern "C" void IDTENTRY111();
extern "C" void IDTENTRY112();
extern "C" void IDTENTRY113();
extern "C" void IDTENTRY114();
extern "C" void IDTENTRY115();
extern "C" void IDTENTRY116();
extern "C" void IDTENTRY117();
extern "C" void IDTENTRY118();
extern "C" void IDTENTRY119();
extern "C" void IDTENTRY120();
extern "C" void IDTENTRY121();
extern "C" void IDTENTRY122();
extern "C" void IDTENTRY123();
extern "C" void IDTENTRY124();
extern "C" void IDTENTRY125();
extern "C" void IDTENTRY126();
extern "C" void IDTENTRY127();
extern "C" void IDTENTRY128();
extern "C" void IDTENTRY129();
extern "C" void IDTENTRY130();
extern "C" void IDTENTRY131();
extern "C" void IDTENTRY132();
extern "C" void IDTENTRY133();
extern "C" void IDTENTRY134();
extern "C" void IDTENTRY135();
extern "C" void IDTENTRY136();
extern "C" void IDTENTRY137();
extern "C" void IDTENTRY138();
extern "C" void IDTENTRY139();
extern "C" void IDTENTRY140();
extern "C" void IDTENTRY141();
extern "C" void IDTENTRY142();
extern "C" void IDTENTRY143();
extern "C" void IDTENTRY144();
extern "C" void IDTENTRY145();
extern "C" void IDTENTRY146();
extern "C" void IDTENTRY147();
extern "C" void IDTENTRY148();
extern "C" void IDTENTRY149();
extern "C" void IDTENTRY150();
extern "C" void IDTENTRY151();
extern "C" void IDTENTRY152();
extern "C" void IDTENTRY153();
extern "C" void IDTENTRY154();
extern "C" void IDTENTRY155();
extern "C" void IDTENTRY156();
extern "C" void IDTENTRY157();
extern "C" void IDTENTRY158();
extern "C" void IDTENTRY159();
extern "C" void IDTENTRY160();
extern "C" void IDTENTRY161();
extern "C" void IDTENTRY162();
extern "C" void IDTENTRY163();
extern "C" void IDTENTRY164();
extern "C" void IDTENTRY165();
extern "C" void IDTENTRY166();
extern "C" void IDTENTRY167();
extern "C" void IDTENTRY168();
extern "C" void IDTENTRY169();
extern "C" void IDTENTRY170();
extern "C" void IDTENTRY171();
extern "C" void IDTENTRY172();
extern "C" void IDTENTRY173();
extern "C" void IDTENTRY174();
extern "C" void IDTENTRY175();
extern "C" void IDTENTRY176();
extern "C" void IDTENTRY177();
extern "C" void IDTENTRY178();
extern "C" void IDTENTRY179();
extern "C" void IDTENTRY180();
extern "C" void IDTENTRY181();
extern "C" void IDTENTRY182();
extern "C" void IDTENTRY183();
extern "C" void IDTENTRY184();
extern "C" void IDTENTRY185();
extern "C" void IDTENTRY186();
extern "C" void IDTENTRY187();
extern "C" void IDTENTRY188();
extern "C" void IDTENTRY189();
extern "C" void IDTENTRY190();
extern "C" void IDTENTRY191();
extern "C" void IDTENTRY192();
extern "C" void IDTENTRY193();
extern "C" void IDTENTRY194();
extern "C" void IDTENTRY195();
extern "C" void IDTENTRY196();
extern "C" void IDTENTRY197();
extern "C" void IDTENTRY198();
extern "C" void IDTENTRY199();
extern "C" void IDTENTRY200();
extern "C" void IDTENTRY201();
extern "C" void IDTENTRY202();
extern "C" void IDTENTRY203();
extern "C" void IDTENTRY204();
extern "C" void IDTENTRY205();
extern "C" void IDTENTRY206();
extern "C" void IDTENTRY207();
extern "C" void IDTENTRY208();
extern "C" void IDTENTRY209();
extern "C" void IDTENTRY210();
extern "C" void IDTENTRY211();
extern "C" void IDTENTRY212();
extern "C" void IDTENTRY213();
extern "C" void IDTENTRY214();
extern "C" void IDTENTRY215();
extern "C" void IDTENTRY216();
extern "C" void IDTENTRY217();
extern "C" void IDTENTRY218();
extern "C" void IDTENTRY219();
extern "C" void IDTENTRY220();
extern "C" void IDTENTRY221();
extern "C" void IDTENTRY222();
extern "C" void IDTENTRY223();
extern "C" void IDTENTRY224();
extern "C" void IDTENTRY225();
extern "C" void IDTENTRY226();
extern "C" void IDTENTRY227();
extern "C" void IDTENTRY228();
extern "C" void IDTENTRY229();
extern "C" void IDTENTRY230();
extern "C" void IDTENTRY231();
extern "C" void IDTENTRY232();
extern "C" void IDTENTRY233();
extern "C" void IDTENTRY234();
extern "C" void IDTENTRY235();
extern "C" void IDTENTRY236();
extern "C" void IDTENTRY237();
extern "C" void IDTENTRY238();
extern "C" void IDTENTRY239();
extern "C" void IDTENTRY240();
extern "C" void IDTENTRY241();
extern "C" void IDTENTRY242();
extern "C" void IDTENTRY243();
extern "C" void IDTENTRY244();
extern "C" void IDTENTRY245();
extern "C" void IDTENTRY246();
extern "C" void IDTENTRY247();
extern "C" void IDTENTRY248();
extern "C" void IDTENTRY249();
extern "C" void IDTENTRY250();
extern "C" void IDTENTRY251();
extern "C" void IDTENTRY252();
extern "C" void IDTENTRY253();
extern "C" void IDTENTRY254();
extern "C" void IDTENTRY255();
unsigned int* AvgCoreSpeed;
unsigned int* NumberIntsPerCore;
extern char* CoreStateList;


typedef void (*IDTFuncEntry)();

typedef void (*IDTEntryType)(unsigned int);
struct IDT::IDTEntry* IDTEntryArray;
IDTEntryType IDTHandlerArray[256];
void IDT::SetISR(unsigned int ISR, IDTEntryType FuncAdding, unsigned char KernelExcusive)
{
    unsigned char DPL = (KernelExcusive)?0:3;
    ISR &= 0xff; // limit it within range
    IDTHandlerArray[ISR] = FuncAdding;
    DPL = (DPL << 5);

    IDTEntryArray[ISR].TypeData &= (~(3 << 5));
    IDTEntryArray[ISR].TypeData |= DPL;


}


unsigned int SyscallSpinLock;
struct IDT::IDTR* IdtrPTR;
char* IDT::CoreIDPerCore;
void GPHandler(unsigned int ISR);
void IDT::InitIDT()
{
    CoreIDPerCore = (char*) Kmalloc::Malloc(256*sizeof(char));
    if(CoreIDPerCore == 0){ Console::PrintString("Cant Alloc Memeory for IDT"); asm("hlt"); }
    for(int i=0; i< 256; i++)
    {
        IDTHandlerArray[i] = 0;
        CoreIDPerCore[i] = 0;
    }
    SyscallSpinLock = 0;
    IDTFuncEntry IDTFuncEntryArray [256] = 
        {
            IDTENTRY0, IDTENTRY1, IDTENTRY2, IDTENTRY3, IDTENTRY4, IDTENTRY5, IDTENTRY6, IDTENTRY7, 
IDTENTRY8, IDTENTRY9, IDTENTRY10, IDTENTRY11, IDTENTRY12, IDTENTRY13, IDTENTRY14, IDTENTRY15, 
IDTENTRY16, IDTENTRY17, IDTENTRY18, IDTENTRY19, IDTENTRY20, IDTENTRY21, IDTENTRY22, IDTENTRY23, 
IDTENTRY24, IDTENTRY25, IDTENTRY26, IDTENTRY27, IDTENTRY28, IDTENTRY29, IDTENTRY30, IDTENTRY31, 
IDTENTRY32, IDTENTRY33, IDTENTRY34, IDTENTRY35, IDTENTRY36, IDTENTRY37, IDTENTRY38, IDTENTRY39, 
IDTENTRY40, IDTENTRY41, IDTENTRY42, IDTENTRY43, IDTENTRY44, IDTENTRY45, IDTENTRY46, IDTENTRY47, 
IDTENTRY48, IDTENTRY49, IDTENTRY50, IDTENTRY51, IDTENTRY52, IDTENTRY53, IDTENTRY54, IDTENTRY55, 
IDTENTRY56, IDTENTRY57, IDTENTRY58, IDTENTRY59, IDTENTRY60, IDTENTRY61, IDTENTRY62, IDTENTRY63, 
IDTENTRY64, IDTENTRY65, IDTENTRY66, IDTENTRY67, IDTENTRY68, IDTENTRY69, IDTENTRY70, IDTENTRY71, 
IDTENTRY72, IDTENTRY73, IDTENTRY74, IDTENTRY75, IDTENTRY76, IDTENTRY77, IDTENTRY78, IDTENTRY79, 
IDTENTRY80, IDTENTRY81, IDTENTRY82, IDTENTRY83, IDTENTRY84, IDTENTRY85, IDTENTRY86, IDTENTRY87, 
IDTENTRY88, IDTENTRY89, IDTENTRY90, IDTENTRY91, IDTENTRY92, IDTENTRY93, IDTENTRY94, IDTENTRY95, 
IDTENTRY96, IDTENTRY97, IDTENTRY98, IDTENTRY99, IDTENTRY100, IDTENTRY101, IDTENTRY102, IDTENTRY103, 
IDTENTRY104, IDTENTRY105, IDTENTRY106, IDTENTRY107, IDTENTRY108, IDTENTRY109, IDTENTRY110, IDTENTRY111, 
IDTENTRY112, IDTENTRY113, IDTENTRY114, IDTENTRY115, IDTENTRY116, IDTENTRY117, IDTENTRY118, IDTENTRY119, 
IDTENTRY120, IDTENTRY121, IDTENTRY122, IDTENTRY123, IDTENTRY124, IDTENTRY125, IDTENTRY126, IDTENTRY127, 
IDTENTRY128, IDTENTRY129, IDTENTRY130, IDTENTRY131, IDTENTRY132, IDTENTRY133, IDTENTRY134, IDTENTRY135, 
IDTENTRY136, IDTENTRY137, IDTENTRY138, IDTENTRY139, IDTENTRY140, IDTENTRY141, IDTENTRY142, IDTENTRY143, 
IDTENTRY144, IDTENTRY145, IDTENTRY146, IDTENTRY147, IDTENTRY148, IDTENTRY149, IDTENTRY150, IDTENTRY151, 
IDTENTRY152, IDTENTRY153, IDTENTRY154, IDTENTRY155, IDTENTRY156, IDTENTRY157, IDTENTRY158, IDTENTRY159, 
IDTENTRY160, IDTENTRY161, IDTENTRY162, IDTENTRY163, IDTENTRY164, IDTENTRY165, IDTENTRY166, IDTENTRY167, 
IDTENTRY168, IDTENTRY169, IDTENTRY170, IDTENTRY171, IDTENTRY172, IDTENTRY173, IDTENTRY174, IDTENTRY175, 
IDTENTRY176, IDTENTRY177, IDTENTRY178, IDTENTRY179, IDTENTRY180, IDTENTRY181, IDTENTRY182, IDTENTRY183, 
IDTENTRY184, IDTENTRY185, IDTENTRY186, IDTENTRY187, IDTENTRY188, IDTENTRY189, IDTENTRY190, IDTENTRY191, 
IDTENTRY192, IDTENTRY193, IDTENTRY194, IDTENTRY195, IDTENTRY196, IDTENTRY197, IDTENTRY198, IDTENTRY199, 
IDTENTRY200, IDTENTRY201, IDTENTRY202, IDTENTRY203, IDTENTRY204, IDTENTRY205, IDTENTRY206, IDTENTRY207, 
IDTENTRY208, IDTENTRY209, IDTENTRY210, IDTENTRY211, IDTENTRY212, IDTENTRY213, IDTENTRY214, IDTENTRY215, 
IDTENTRY216, IDTENTRY217, IDTENTRY218, IDTENTRY219, IDTENTRY220, IDTENTRY221, IDTENTRY222, IDTENTRY223, 
IDTENTRY224, IDTENTRY225, IDTENTRY226, IDTENTRY227, IDTENTRY228, IDTENTRY229, IDTENTRY230, IDTENTRY231, 
IDTENTRY232, IDTENTRY233, IDTENTRY234, IDTENTRY235, IDTENTRY236, IDTENTRY237, IDTENTRY238, IDTENTRY239, 
IDTENTRY240, IDTENTRY241, IDTENTRY242, IDTENTRY243, IDTENTRY244, IDTENTRY245, IDTENTRY246, IDTENTRY247, 
IDTENTRY248, IDTENTRY249, IDTENTRY250, IDTENTRY251, IDTENTRY252, IDTENTRY253, IDTENTRY254, IDTENTRY255

}; // WARNING this HAS to be inside this func, if its not it will load junk data for the ISR runtime locasions
    Console::PrintString("Starting IDT\n");
    IDTEntryArray = (struct IDTEntry*)Kmalloc::Malloc(sizeof(IDTEntry)*256);
    IdtrPTR = (struct IDTR*)Kmalloc::Malloc(sizeof(IDTR));
    if(IDTEntryArray == 0 || IdtrPTR == 0)
    {
        Console::PrintString("Failed To Alloc Memory for IDT\n");
        asm("hlt");
    }


    for(int i=0; i< 256; i++)
    {
        struct IDTEntry TmpTable;
        TmpTable.FuncAddrLow = (unsigned short)(((unsigned int)IDTFuncEntryArray[i]) & 0xFFFF);
        TmpTable.FuncAddrHigh = (unsigned short)(((unsigned int)IDTFuncEntryArray[i] >> 16) & 0xFFFF);
        TmpTable.Zero = 0;
        TmpTable.Segment = 8;
        TmpTable.TypeData = 0x8E;
        IDTEntryArray[i]=TmpTable;
    }

    IdtrPTR->SizeBytes = (sizeof(IDTEntry)*256)-1;
    IdtrPTR->Addr = (unsigned int)IDTEntryArray;
    
    IDT::CpuUnderFlowHandler = 0;

    asm volatile("lidt (%0)" :: "a"(IdtrPTR));

    SetISR(0x80, TestSystemcall, 0);
    NumberInts = 0xffff;

    AvgCoreSpeed = 0;
    NumberIntsPerCore = 0;
    CurrentCoreRunning = 1;
    StartedThread = 0;

    //asm("int $0x80");

}

void TestSystemcall(unsigned int ISR)
{
    //Console::PrintString("This is a test of the new ISR system!\n", 1);

    Console::PrintString("Got Syscall\n");

}
#include "Cpuid.hpp"
extern void* StartStack;
unsigned int EEFlagsOverwride = 0;
void GPHandler(unsigned int ISR)
{
    Console::PrintString("Got GP Fault\n");
    unsigned int EEFlags;
    if(EEFlagsOverwride != 0)
    {
        EEFlags = EEFlagsOverwride;
        EEFlagsOverwride = 0;
    }
    else{
        EEFlags = ((unsigned int*)StartStack)[0];
    }
    unsigned int ErrorCode = ((unsigned int*)StartStack)[1];
    Console::PrintString("EEFlags:");
    Console::PrintInt(EEFlags, 0);
    Console::PrintString("|Error Code:");
    Console::PrintInt(ErrorCode, 0);

    Console::PrintString("\nDumping MSR:");
    unsigned int MsrL;
    unsigned int MsrH;
    GetMSR(0x1b, &MsrL, &MsrH);

    Console::PrintString("\nLo:");
    Console::PrintInt(MsrL, 1);
    Console::PrintString("\nHi:");
    Console::PrintInt(MsrH, 1);
    Console::PrintString("\n");

    if(MsrL & (1 << 11)){
        Console::PrintString("Apic Enabled|");
    }
    if(MsrL & (1 << 10))
    {
        Console::PrintString("x2APIC Mode Enabled|");
    }

    asm("hlt");

}
extern "C" void StartThread(unsigned int AppPtr,unsigned int StackPtr);
extern "C" void ReStartThread(unsigned int StackPtr);
IDTEntryType IDT::CpuIntStormHandler;
extern unsigned int NumberCores;
extern unsigned char* CoreIDS;
unsigned char LastRecordedSecond = 0;
unsigned int* CorePrintInfo;
extern char** CoreStackPtrs; 
extern unsigned int* CoreCurrentThreadID;

IDTFuncEntry IDT::CpuUnderFlowHandler;
 
void MoveToStartStack(unsigned int CoreIndex)
{
    /*Console::PrintString("Core Index:");
    Console::PrintInt(CoreIndex, 0);
    Console::PrintString("\n");*/
    CoreStateList[CoreIndex] = 4; // make sure the ISR stack resets
    Apic::WriteLAPICReg(0xb0, 0);
    Apic::WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
    Apic::WriteLAPICReg(0x3E0, 0x3);
    Apic::WriteLAPICReg(0x380,(Apic::ReadLAPICReg(0x380)));
    Apic::WriteLAPICReg(0xf0, 0x100); // reset the core timer
    ReStartThread((unsigned int)CoreStackPtrs[CoreIndex]);
    while(1);
}





extern unsigned int NumberThreads;
extern struct ThreadObject* ThreadArray;


#include "Syscall/Syscall.hpp"

extern unsigned char CreatingNewThread;

unsigned int CoreStateSpinLock = 0;

extern "C" void GetIdtRegistors(unsigned int Stack, unsigned int* Registors);

extern "C" void IDThandler(unsigned int ISR, unsigned int ISRStartingStack)
{
    Apic::WriteLAPICReg(0xf0, 0xff);

    if(ISR > 255)
    {
        Console::PrintString("Broken ISR Num\n");
        while(1);
    }

    IO::outb(0x70, 0);
    unsigned char Second = IO::inb(0x71);
    unsigned char SecondChanged = 0;
    if(Second != LastRecordedSecond)
    {
        LastRecordedSecond = Second;
        SecondChanged = 1;
        //Console::PrintString("Seccond Passed\n");
        
    }
    
    int CoreIndex = -1;

    unsigned int ebx, unused;
    Cpuid(0x1, &unused, &ebx, &unused, &unused);

    unsigned char LApicID = ((ebx >> 24) & 0xff);

    for(int i=0; i< NumberCores; i++){
        if(CoreIDS[i] == LApicID)
        {
            CoreIndex = i;
            break;
        }
    }
    if(CoreIndex == -1)
    {
        Console::PrintString("Unknown CoreID Error\n");
        while(1);
    }


        //Console::PrintString("Core ");
        //Console::PrintInt(CoreIndex, 0);
        //Console::PrintString(" Is Waiting for core ");
        //Console::PrintInt(CurrentCoreRunning, 0);
        //Console::PrintString(" To Finnish\n");
        if(CoreStateList == 0)
        {
            Console::PrintString("ERROR BROKEN STATE LIST\n");
            while(1);
        }
        if(CoreStateList[CoreIndex] == 4)
        {
            CoreStateList[CoreIndex] = 5; // tell the BSC that it started up without issue
            CoreStackPtrs[CoreIndex] = (char*)ISRStartingStack; // save the stack for later
            //Console::PrintString("Setting Core("); Console::PrintInt(CoreIndex, 0); Console::PrintString(") Stack\n");
            Apic::WriteLAPICReg(0xb0, 0);
            Apic::WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
            Apic::WriteLAPICReg(0x3E0, 0x3);
            Apic::WriteLAPICReg(0x380,(Apic::ReadLAPICReg(0x380)));
            Apic::WriteLAPICReg(0xf0, 0x100); // reset the core timer
            return;
        }
    

    if(AvgCoreSpeed != 0 || CorePrintInfo != 0)
    {
        AvgCoreSpeed[CoreIndex]++;
        CorePrintInfo[CoreIndex]++;
    }


    //while(CoreStateSpinLock);
    //while(CurrentCoreRunning != CoreIndex);

    if(CoreStackPtrs[CoreIndex] == 0){ CoreStackPtrs[CoreIndex] = (char*)ISRStartingStack;}
    
    //((char*)(0xb8000))[CoreIndex*2]++;

    if(AvgCoreSpeed[CoreIndex] >400)
    {
        IDT::CpuIntStormHandler(ISR);
        AvgCoreSpeed[CoreIndex] = 0;
    }
    if(NumberIntsPerCore[CoreIndex] < 200 && NumberIntsPerCore != 0 && CoreIndex != -1)
    {
        if(IDT::CpuUnderFlowHandler != 0){IDT::CpuUnderFlowHandler();}
        NumberIntsPerCore[CoreIndex] = 0xFFFF;
    }
    if(IDTHandlerArray[ISR] != 0)
    {
        IDTHandlerArray[ISR](ISR);
    }
    else if(ISR < 0x20)
    {
        Apic::WriteLAPICReg(0xf0, 0xff); 
        Console::PrintString("Core ");
        Console::PrintInt(CoreIndex, 0);
        Console::PrintString(" Got ERROR:");
        Console::PrintInt(ISR, 1);
        Console::PrintString("\n");
        CurrentCoreRunning = 0xFF;
        Paging::PageSpinLock = 1;

        Console::PrintString("Attempting Data Dump\n");
        Console::PrintString("Start Start PTR:");
        Console::PrintInt((unsigned int)ISRStartingStack-4, 1);
        Console::PrintString("\n");

        if(((unsigned int)ISRStartingStack-4) < 0x200000) // cant be under 2mb as thats where the malloc starts
        {
            Console::PrintString("Stack Is Broken, Likey Caused Error\n");
        }

        Apic::WriteLAPICReg(0xf0, 0xff); // mask the LAPIC

        Console::PrintString("Rebooting Soon\n");

        for(int i=0; i< 10; i++)
        {
            for(int j=0; j< 0xFFFFFF; j++)
            {
                IO::iowait();
            }
            Console::PrintString(".");
            
        }

        asm volatile("lidt (%0)" :: "a"(0)); // break the IDT

        asm volatile("int $0x80"); // force a reboot

        
        while(1){
            asm("cli");
            asm("hlt");
        }
    }


    if(CoreCurrentThreadID != 0 && ThreadArray != 0 && NumberThreads != 0 /*&& ISR == 0x32*/)
    {
        Spinlock::WaitSet(&Thread::SpinLock);
        ThreadObject TmpObj = ThreadArray[CoreCurrentThreadID[CoreIndex]];
        if(!TmpObj.FirstStart && TmpObj.CoreUsing == CoreIndex)
        {
            if(TmpObj.ThreadState == 0){
                ThreadArray[CoreCurrentThreadID[CoreIndex]].StackPTR = ISRStartingStack;
            }
            
        }
        Thread::SpinLock = 0;
    }
    if(ISR == 0x80 && (ThreadArray != 0))
    {
        Spinlock::WaitSet(&SyscallSpinLock);
        unsigned int* Registors = (unsigned int*)(ISRStartingStack+4);
        
        //Console::PrintString("Got Syscall:");
        //Console::PrintInt(Registors[0], 0);
        //Console::PrintString("\n");

        if(Registors[0] < 0xFFFF ) // theres only 0xFFFF entrys
        {
            if(SysCalls::SysCallList[Registors[0]] != 0)
            {
                SysCalls::SysCallList[Registors[0]](Registors, &ThreadArray[CoreCurrentThreadID[CoreIndex]]);
                SyscallSpinLock = 0;
            }
        }
        
        
    }



    Spinlock::WaitSet(&Ps2SpinLock);
    Ps2::OnPs2CheckCycle();
    Ps2SpinLock = 0;
    if(ISR != 0x32 && ThreadArray != 0){ThreadArray[CoreCurrentThreadID[CoreIndex]].NumberSyscallsPerSecond++;} // skip LAPIC timer ints
    if(ISR == 0x32)
    {
        Apic::WriteLAPICReg(0xb0, 0);
    } 
    

    if(CoreIndex != -1)
    {
        if(AvgCoreSpeed == 0)
        {
            AvgCoreSpeed = (unsigned int*) Kmalloc::Malloc(4*NumberCores);
            NumberIntsPerCore = (unsigned int*) Kmalloc::Malloc(4*NumberCores);
            CorePrintInfo = (unsigned int*) Kmalloc::Malloc(4*NumberCores);
            for(int i=0; i < NumberCores; i++){ NumberIntsPerCore[i]=0xFFFF; }
        }
        if(AvgCoreSpeed == 0 || NumberIntsPerCore == 0 || CorePrintInfo == 0)
        {
            CurrentCoreRunning++;
            if(CurrentCoreRunning >= NumberCores)
            {
                CurrentCoreRunning = 0;
            }
            
        }
        else if(SecondChanged){
            
            for(int i=0; i< NumberCores; i++)
            { 
                /*Console::PrintString("Cpu Performace Report(by the second)[");
                Console::PrintInt(i, 0);
                Console::PrintString("]:");
                Console::PrintInt(CorePrintInfo[i], 0);
                Console::PrintString("\n"); // good for debuging, not for use
                */
                NumberIntsPerCore[i]=AvgCoreSpeed[i];
                AvgCoreSpeed[i] = 0; 
                CorePrintInfo[i]=0;
            }
            for(int i=0; i< NumberThreads; i++)
            {
                ThreadArray[i].NumberSyscallsPerSecond = 0;
            }

        }
        AvgCoreSpeed[CoreIndex]++;
        CorePrintInfo[CoreIndex]++;
    }
    
    if(CoreCurrentThreadID != 0 && ThreadArray != 0 && NumberThreads != 0 /*&& ISR == 0x32*/)
    {
        Spinlock::WaitSet(&Thread::SpinLock);
        
        
        
        
        
        
      

        unsigned int ThreadIndex = 0;
        while(1){
            CoreCurrentThreadID[CoreIndex]++;
            if(CoreCurrentThreadID[CoreIndex] > (NumberThreads-1))
            {
                CoreCurrentThreadID[CoreIndex] = 0;
            }
            if(ThreadArray[CoreCurrentThreadID[CoreIndex]].CoreUsing == CoreIndex)
            {
                 
                //if(ThreadArray[CoreCurrentThreadID[CoreIndex]].ThreadState == 0){ (*((char*)0xb8000+300+(CoreIndex*2)))++;}
                break;
            }
            ThreadIndex++;
            if(ThreadIndex >= (NumberThreads)){break;}
        }

        ThreadObject TmpObj = ThreadArray[CoreCurrentThreadID[CoreIndex]];
        if(TmpObj.CoreUsing == CoreIndex && (TmpObj.ThreadState == 4))
        {
            
            Console::PrintString("Thread Restarting\n");
            Apic::WriteLAPICReg(0xb0, 0);
            Apic::WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
            Apic::WriteLAPICReg(0x3E0, 0x3);
            Apic::WriteLAPICReg(0x380,(Apic::ReadLAPICReg(0x380)));
            Apic::WriteLAPICReg(0xf0, 0x100); // reset the core timer
            ThreadArray[CoreCurrentThreadID[CoreIndex]].ThreadState = 0;
            Thread::SpinLock = 0;
            ReStartThread(TmpObj.StackPTR);
            
        }
        if(TmpObj.CoreUsing == CoreIndex && (TmpObj.FirstStart) && (TmpObj.ThreadState == 0))
        {
            // run the StartThread
            // func
            Console::PrintString("Starting Func\n");
            ThreadArray[CoreCurrentThreadID[CoreIndex]].FirstStart = 0;
            Apic::WriteLAPICReg(0xb0, 0);
            Apic::WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
            Apic::WriteLAPICReg(0x3E0, 0x3);
            Apic::WriteLAPICReg(0x380,(Apic::ReadLAPICReg(0x380)));
            Apic::WriteLAPICReg(0xf0, 0x100); // reset the core timer
            Thread::SpinLock = 0;
            StartThread((unsigned int)TmpObj.AppStart, TmpObj.StackPTR);
            
            while(1);
        }
        if((TmpObj.CoreUsing == CoreIndex) && (!TmpObj.FirstStart) && (TmpObj.ThreadState == 0))
        {
            // RestartThread func
           // (*((char*)(0xb8000+200+(CoreCurrentThreadID[CoreIndex]*2))))++;
            Apic::WriteLAPICReg(0xb0, 0);
            Apic::WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
            Apic::WriteLAPICReg(0x3E0, 0x3);
            Apic::WriteLAPICReg(0x380,(Apic::ReadLAPICReg(0x380)));
            Apic::WriteLAPICReg(0xf0, 0x100); // reset the core timer
            Thread::SpinLock = 0;
            ReStartThread(TmpObj.StackPTR);
            while(1);
        }

        if(TmpObj.CoreUsing != CoreIndex || TmpObj.ThreadState != 0)
        {
            Thread::SpinLock = 0;
            MoveToStartStack(((unsigned int)CoreIndex));
        }

        
            
        
    }

    
    
    
    
    



    

    
    
    CurrentCoreRunning++; // set the order index to the next item
    if(CurrentCoreRunning >= NumberCores)
    {
        CurrentCoreRunning = 0;
    }
    Apic::WriteLAPICReg(0xf0, 0x100);
}


