#include "CommandLine.hpp"
#include "../../console/console.hpp"
#include "../../Exec/Thread.hpp"
#include "../NeededTools/WatchDog.hpp"
#include "../../Ps2/Ps2.hpp"
#include "../RamDiskMgr.hpp"


void CommandLoop()
{
    char InputBuffer[512] = {0};

    const char* TmpStr = "\n$";
    asm("int $0x80" :: "S"(0), "ebx"(TmpStr));

    struct Ps2Input
    {
        void* Buffer;
        unsigned int NumberBytes;
    };

    Ps2Input Ps2SyscallInput = 
    {
        &InputBuffer,
        500
    };

    asm("int $0x80" :: "S"(0x20), "ebx"(&Ps2SyscallInput));  

    unsigned int InputSize= 0;
    char* InputTmp = (InputBuffer+1);
    while(InputTmp[InputSize] != 0){InputSize++;}

    if(!(InputTmp[InputSize-1] == 'e'  && InputTmp[InputSize-2] == 'x' && InputTmp[InputSize-3] == 'e' && InputTmp[InputSize-4] == 'j' && InputTmp[InputSize-5] == '.'))
    {
            InputTmp[InputSize+5] = 0;
            InputTmp[InputSize+4] = 'e'; 
            InputTmp[InputSize+3] = 'x';  
            InputTmp[InputSize+2] = 'e';  
            InputTmp[InputSize+1] = 'j'; 
            InputTmp[InputSize] = '.';
    }


    InputBuffer[0] = '/';
    const char* Tmp  ="\nOpening App:";
    asm("int $0x80" :: "S"(0), "ebx"(Tmp));
    asm("int $0x80" :: "S"(0), "ebx"(InputBuffer));
    unsigned int Output;
    asm("int $0x80" : "=ebx"(Output): "S"(0x11), "ebx"(InputBuffer)); // freeze the thread until later

    if(Output == 0){ return;}
    //Console::PrintString("Got Output:");
    //Console::PrintInt(Output, 0);
    //Console::PrintString("\n");

    const char* TestPrint = "The App Has Closed\n";
    asm("int $0x80" :: "S"(0), "ebx"(TestPrint));

}


void CommandLine::CommandLineEntry()
{
    //Console::PrintString("Starting Background Watchdog Thread...\n");
    //Console::PrintString("Started Thread\n");
    asm("int $0x80" :: "S"(0x08));

    const char* PrintString = "\nWelcome to Jenny OS(v0.0.0)\nJenny OS comes with ZERO WARRANTY for ANY use(GPL-3).\nJenny OS is a hobby OS.\nIf you find any bugs please email me at transjennyyyyy@gmail.com Run contri for more info\n";
    asm("int $0x80" :: "S"(0), "ebx"(PrintString));
    unsigned int NumberAppsRan  = 0;
    
    for(;;)
    {
        CommandLoop();
    }

}
