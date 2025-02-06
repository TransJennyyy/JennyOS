#include "Thread.hpp"
#include "../Apic/Apic.hpp"
#include "../malloc/malloc.hpp"
#include "../console/console.hpp"
#include "../SpinLock.hpp"
unsigned int NumberThreads;

extern unsigned int* CoreCurrentThreadID;

struct ThreadObject* ThreadArray;
 
extern unsigned int NumberCores;
extern unsigned char* CoreIDS;

extern unsigned int StartingThread;
extern unsigned char StartedThread;

unsigned char CreatingNewThread;

unsigned int Thread::SpinLock;

unsigned int Thread::InitThread(void* AppStart, unsigned int Priority, unsigned char IsBGTask)
{
    Apic::WriteLAPICReg(0xf0, 0xff); // mask all LAPIC ints
    asm("cli");
    Spinlock::WaitSet(&SpinLock);

    unsigned int CoreIndexUsing = 0; // will try to find the core with the lowist amout of apps, otherwise put on core 0

    unsigned int* NumberThreadPerCore = (NumberThreads != 0)?(unsigned int*) Kmalloc::Malloc(NumberCores*4):0;
    for(int i=0; i< NumberCores; i++){ NumberThreadPerCore[i]= 0; }
    if(NumberThreadPerCore != 0){ // if this fails we just stick to core 0

        for(int i=0; i< NumberThreads; i++)
        {
            //if(ThreadArray[i].ThreadPaused){continue;} // we dont count paused cores in how much a core is currently doing
            NumberThreadPerCore[ThreadArray[i].CoreUsing]++;
        }

        for(int i=0; i< NumberCores; i++)
        {
            if(NumberThreadPerCore[CoreIndexUsing] > NumberThreadPerCore[i])
            {
                CoreIndexUsing = i;
            }
            //Console::PrintString("Found ");
            //Console::PrintInt(NumberThreadPerCore[i], 0);
            //Console::PrintString(" Threads On Core\n");
        }
    }

    Kmalloc::Free(NumberThreadPerCore);
    //CoreIndexUsing = 0;
    unsigned int BackupIndex = CoreIndexUsing+1;
    if(BackupIndex >= NumberCores)
    {
        BackupIndex = 0;
    }

    unsigned int OpenIndex = 0xFFFFFFFF;

    for(int i=0; i < NumberThreads; i++)
    {
        if(ThreadArray[i].ThreadState == 2)
        {
            OpenIndex = i;
            break;
        }
    }

    if(OpenIndex != 0xFFFFFFFF)
    {
        Console::PrintString("Reusing Dead Table\n");
        ThreadArray[OpenIndex].AppStart = AppStart;
        ThreadArray[OpenIndex].BackupCore = BackupIndex;
        ThreadArray[OpenIndex].CoreUsing = CoreIndexUsing;
        ThreadArray[OpenIndex].StackPTR = (unsigned int) Kmalloc::Malloc(0x1000);
        if(ThreadArray[OpenIndex].StackPTR == 0)
        {
            Console::PrintString("Alloc Issue With Thread\n");
            while(1);
        }
        ThreadArray[OpenIndex].StackPTR += (0x1000);
        ThreadArray[OpenIndex].FirstStart =1;
        ThreadArray[OpenIndex].Priority = Priority;
        ThreadArray[OpenIndex].ThreadCloseMsg = 0;
        ThreadArray[OpenIndex].ThreadPaused = 0;
        ThreadArray[OpenIndex].ThreadWaitTime = 0;
        ThreadArray[OpenIndex].NumberSyscallsPerSecond = 0;
        ThreadArray[OpenIndex].BgTask = IsBGTask;
        ThreadArray[OpenIndex].ThreadState = 0;
        ThreadArray[OpenIndex].ThreadParentIndex = 0;
        CreatingNewThread = 0;
        SpinLock = 0;
        return OpenIndex;
    }


    struct ThreadObject* TmpArray = (struct ThreadObject*) Kmalloc::Malloc(sizeof(struct ThreadObject)*(NumberThreads+1));
    if(TmpArray == 0)
    {
        Console::PrintString("malloc error");
        while(1);
    }
    //unsigned char* FirstStartThreadTmp = (unsigned char*) Kmalloc::Malloc(NumberThreads+1);

    for(int i=0; i< NumberThreads; i++)
    {
        TmpArray[i] = ThreadArray[i];
        //FirstStartThreadTmp[i] = FirstStartThread[i];

    }

    TmpArray[NumberThreads].AppStart = AppStart;
    TmpArray[NumberThreads].CoreUsing = CoreIndexUsing;
    TmpArray[NumberThreads].BackupCore = BackupIndex;
    TmpArray[NumberThreads].NumberSyscallsPerSecond = 0;
    TmpArray[NumberThreads].Priority = Priority; 
    TmpArray[NumberThreads].StackPTR = (unsigned int) Kmalloc::Malloc(0x1000);
    TmpArray[NumberThreads].FirstStart =1;
    TmpArray[NumberThreads].ThreadPaused = 0;
    TmpArray[NumberThreads].ThreadWaitTime = 0;
    TmpArray[NumberThreads].ThreadCloseMsg = (void*)0;
    TmpArray[NumberThreads].BgTask = IsBGTask;
    TmpArray[NumberThreads].ThreadParentIndex = 0;
    TmpArray[NumberThreads].ThreadState = 0;
    if(TmpArray[NumberThreads].StackPTR == 0)
    {
        Console::PrintString("Error Cant Alloc Stack For Thread\n");
        while(1);
    }
    TmpArray[NumberThreads].StackPTR += (0x1000); // offset stack for use

    //Console::PrintString("Created Thread PID:");
    //Console::PrintInt(NumberThreads,0);

    //Console::PrintString(" On Core ");
    //Console::PrintInt(CoreIndexUsing, 0);
    //Console::PrintString("\n");

    //if(NumberThreads > 0)
    //{
    //    while(1);
    //}


    Kmalloc::Free(ThreadArray);
    //Kmalloc::Free(FirstStartThread);
    NumberThreads++;
    ThreadArray = TmpArray;
    //FirstStartThread = FirstStartThreadTmp;
    
    CreatingNewThread = 0;

    

    //Apic::WriteLAPICReg(0xb0, 0);
    //Apic::WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
    //Apic::WriteLAPICReg(0x3E0, 0x3);
    //Apic::WriteLAPICReg(0x380,(Apic::ReadLAPICReg(0x380)));
    //Apic::WriteLAPICReg(0xf0, 0x100); // reset the core timer
    //asm("sti");
    SpinLock = 0;
    return NumberThreads-1;


}