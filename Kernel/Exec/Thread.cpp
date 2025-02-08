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
        ThreadArray[OpenIndex].ThreadWaitTime = 0;
        ThreadArray[OpenIndex].NumberSyscallsPerSecond = 0;
        ThreadArray[OpenIndex].ThreadState = 0;
        ThreadArray[OpenIndex].ThreadParentIndex = 0;
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
    TmpArray[NumberThreads].ThreadWaitTime = 0;
    TmpArray[NumberThreads].ThreadCloseMsg = (void*)0;
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
    

    

    //Apic::WriteLAPICReg(0xb0, 0);
    //Apic::WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
    //Apic::WriteLAPICReg(0x3E0, 0x3);
    //Apic::WriteLAPICReg(0x380,(Apic::ReadLAPICReg(0x380)));
    //Apic::WriteLAPICReg(0xf0, 0x100); // reset the core timer
    //asm("sti");
    SpinLock = 0;
    return NumberThreads-1;


}
extern "C" void StartThread(unsigned int AppPtr,unsigned int StackPtr);
extern "C" void ReStartThread(unsigned int StackPtr);


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
void Thread::SwitchTask(unsigned int CoreIndex, unsigned int ISRStartingStack)
{

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
        if(TmpObj.CoreUsing == CoreIndex && (TmpObj.ThreadState == ThreadStateRestarting))
        {
            
            //Console::PrintString("Thread Restarting\n");
            Apic::WriteLAPICReg(0xb0, 0);
            Apic::WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
            Apic::WriteLAPICReg(0x3E0, 0x3);
            Apic::WriteLAPICReg(0x380,(Apic::ReadLAPICReg(0x380)));
            Apic::WriteLAPICReg(0xf0, 0x100); // reset the core timer
            ThreadArray[CoreCurrentThreadID[CoreIndex]].ThreadState = ThreadStateNormal;
            Thread::SpinLock = 0;
            ReStartThread(TmpObj.StackPTR);
            
        }
        if(TmpObj.CoreUsing == CoreIndex && (TmpObj.FirstStart) && (TmpObj.ThreadState == ThreadStateNormal))
        {
            // run the StartThread
            // func
            //Console::PrintString("Starting Func\n");
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
        if((TmpObj.CoreUsing == CoreIndex) && (!TmpObj.FirstStart) && (TmpObj.ThreadState == ThreadStateNormal))
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

        if(TmpObj.CoreUsing != CoreIndex || TmpObj.ThreadState != ThreadStateNormal)
        {
            Thread::SpinLock = 0;
            MoveToStartStack(((unsigned int)CoreIndex));
        }

        
            
        
    }

}
