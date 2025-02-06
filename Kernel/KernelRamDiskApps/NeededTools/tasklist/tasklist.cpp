
#include "tasklist.hpp"
#include "../../../Exec/Thread.hpp"
#include "../../../console/console.hpp"


const char* WelcomeString = "Task List App Started\n";
const char* TableTop = "PID  |  Core  | Fallback Core | Syscalls per sec\n";
const char* BetweenBar = "  |  ";
const char* EndOfList = "No More threads\n";
const char* NewLine = "\n";
const char* InvaildStr = "INVAILD";
extern unsigned int NumberThreads;
extern struct ThreadObject* ThreadArray;
extern unsigned int NumberCores;

void TaskList::Entry()
{
    asm("int $0x80" :: "S"(0), "ebx"(WelcomeString));
    asm("int $0x80" :: "S"(0), "ebx"(TableTop));
    for(int i=0; i< NumberThreads; i++) // for each thread we list, PID(i) | Core | Backup Core | NumberSyscalls Per Second
    {
        if(!ThreadArray[i].ThreadPaused) { // dont show closed apps
            Console::PrintInt(i, 0);
            asm("int $0x80" :: "S"(0), "ebx"(BetweenBar));
            Console::PrintInt(ThreadArray[i].CoreUsing, 0);
            asm("int $0x80" :: "S"(0), "ebx"(BetweenBar));
            Console::PrintInt(ThreadArray[i].BackupCore, 0);
            asm("int $0x80" :: "S"(0), "ebx"(BetweenBar));
            Console::PrintInt(ThreadArray[i].NumberSyscallsPerSecond, 0);
            asm("int $0x80" :: "S"(0), "ebx"(NewLine));
        }
        
        
    }
    asm("int $0x80" :: "S"(0), "ebx"(EndOfList));
    asm("int $0x80" :: "S"(0x30)); // sys exit
    while(1);
}