#include "Syscall.hpp"
#include "../idt.hpp"
#include "../console/console.hpp"
#include "../Exec/Thread.hpp"
#include "../FS/FileSystem.hpp"
#include "../malloc/malloc.hpp"
#include "../Ps2/Ps2.hpp"
#include "../KernelRamDiskApps/RamDiskMgr.hpp"
#include "../Apic/Apic.hpp"
#include "../SpinLock.hpp"
SysCallFunc SysCalls::SysCallList[0xFFFF];


void PrintSyscall(unsigned int* Registors, void* ThreadOBJVPtr)
{
    struct ThreadObject* ThreadOBJ = (struct ThreadObject*)ThreadOBJVPtr;
    unsigned int PrintAddress = Registors[3];
    //PrintAddress += (unsigned int)ThreadOBJ->AppStart;
    Console::PrintString((char*)PrintAddress);
}

extern unsigned int NumberThreads;
extern struct ThreadObject* ThreadArray;
extern "C" void StartThread(unsigned int AppPtr,unsigned int StackPtr);
extern unsigned int StartingThread;
extern unsigned int* CoreCurrentThreadID;
void LoadApp(unsigned int* Registors, void* ThreadObjPtr)
{
    struct ThreadObject* ThreadOBJ = (struct ThreadObject*)ThreadObjPtr;
    unsigned int PrintAddress = Registors[3];

    char CopyedString[512];
    unsigned int NumberBytesStr = FS::GetFilePathSize((char*)PrintAddress);
    for(int i=0; i< NumberBytesStr; i++)
    {
        CopyedString[i] = ((char*)PrintAddress)[i];
    }
    CopyedString[NumberBytesStr] = 0;


    //Console::PrintString("Starting New App At Path:");
    //Console::PrintString((char*)CopyedString);
    //Console::PrintString("\n");

    

   void* FilePtr = RamDiskMgr::ReadFile(CopyedString);
   if(FilePtr == 0)
   {
        Console::PrintString("No Such File Or Dir\n");
        Registors[3] = 0; // set ebx to zero
        return;
   }

    

    Registors[3] = Thread::InitThread(FilePtr, 0, 1);

    //Console::PrintString("Opened Thread Now ");
    //Console::PrintInt(NumberThreads , 0);
    //Console::PrintString(" Threads Open\n");    
}

void LoadAppNonBG(unsigned int* Registors, void* ThreadObjPtr)
{
    struct ThreadObject* ThreadOBJ = (struct ThreadObject*)ThreadObjPtr;
    unsigned int PrintAddress = Registors[3];
    //PrintAddress += (unsigned int)ThreadOBJ->AppStart;
    

    //ThreadOBJ->ThreadCloseMsg = (void*)(NumberThreads);

    char CopyedString[512];
    unsigned int NumberBytesStr = FS::GetFilePathSize((char*)PrintAddress);
    for(int i=0; i< NumberBytesStr; i++)
    {
        CopyedString[i] = ((char*)PrintAddress)[i];
    }
    CopyedString[NumberBytesStr] = 0;


    Console::PrintString("\nStarting New App At Path:");
    Console::PrintString((char*)CopyedString);
    //Console::PrintString("\\");
    //Console::PrintString((char*)PrintAddress);
    Console::PrintString("\n");

    

   void* FilePtr = RamDiskMgr::ReadFile(CopyedString);
   if(FilePtr == 0)
   {
        Console::PrintString("No Such File Or Dir\n");
        Registors[0] = 0; // set ESI to zero
        return;
   }

    unsigned int NewThreadIndex = Thread::InitThread(FilePtr, 0, 0);

    

    Console::PrintString("Opened Thread Now ");
    Console::PrintInt(NewThreadIndex , 0);
    Console::PrintString(" Threads Open\n");   
    
    Spinlock::WaitSet(&Thread::SpinLock);

    Registors[3] = NewThreadIndex;

    ThreadArray[NewThreadIndex].ThreadParentIndex = CoreCurrentThreadID[ThreadOBJ->CoreUsing];
    ThreadArray[CoreCurrentThreadID[ThreadOBJ->CoreUsing]].ThreadState = 3;

    Thread::SpinLock = 0;

    Console::PrintString("Pasuing Thread:");
    Console::PrintInt(CoreCurrentThreadID[ThreadOBJ->CoreUsing], 0);
    Console::PrintString("\n");
}


void GetInput(unsigned int* Registers, void* ThreadObjPtr) // WIP
{
    struct Ps2Syscall
    {
        void* Buffer;
        unsigned int BufferSize;
    };
    struct Ps2Syscall* InputData = (struct Ps2Syscall*)Registers[3]; 
    Ps2::SetPs2InputBuffer(InputData->Buffer+1, InputData->BufferSize, ThreadObjPtr);
}



void ClearScreen(unsigned int* Registors, void* ThreadPbjPtr)
{
    Console::VGAInit();
    struct ThreadObject* AppThread = (struct ThreadObject*)ThreadPbjPtr;
}

void SetExitFunc(unsigned int* Registors, void* ThreadObjPtr)
{
    ThreadArray[NumberThreads-1].ThreadCloseMsg = (void*)Registors[3];
    Console::PrintString("Set Exist Func:");
    Console::PrintInt(Registors[3], 1);
    Console::PrintString("\n");
}

void Exit(unsigned int* Registors, void* ThreadObjPtr)
{
    struct ThreadObject* AppThread = (struct ThreadObject*)ThreadObjPtr;
    Spinlock::WaitSet(&Thread::SpinLock);
    
    unsigned int IndexSetting = AppThread->ThreadParentIndex;

    AppThread->ThreadState = 2; // closed
    if(ThreadArray[AppThread->ThreadParentIndex].ThreadState == 3)
    {
        Console::PrintString("Unpauing thread:");
        Console::PrintInt(IndexSetting, 0);
        ThreadArray[IndexSetting].ThreadState = 4;
    }


    AppThread->StackPTR -= 0x1000;
    Kmalloc::Free((void*)AppThread->StackPTR);
    
    AppThread->StackPTR = 0;
    Kmalloc::Free(AppThread->AppStart);
    Thread::SpinLock = 0;
    Console::PrintString("\nThread Close Request\n");
}

void HasThreadClosed(unsigned int* Reg, void* ThreadObjPtr){

    unsigned int Out = ThreadArray[Reg[3]].ThreadState;
    Reg[3] = Out;    

}


void SysCalls::Init()
{
    for(int i=0; i< 0xFFFF; i++)
    {
        SysCallList[i] = 0;
    }
    SysCallList[0] = PrintSyscall;
    SysCallList[0x08] = ClearScreen;
    SysCallList[0x10] = LoadApp;
    SysCallList[0x11] = LoadAppNonBG;
    SysCallList[0x12] = HasThreadClosed;
    SysCallList[0x30] = Exit;
    SysCallList[0x20] = GetInput;

    //IDT::SetISR(0x80, 0, 0); // should disable the int

}
