#include "CommandLine.hpp"
#include "../../console/console.hpp"
#include "../../Exec/Thread.hpp"
#include "../NeededTools/WatchDog.hpp"
#include "../../Ps2/Ps2.hpp"
#include "../RamDiskMgr.hpp"

void OnInnerAppExit()
{
    Console::PrintString("Inner App Closed\n");

}


void CommandLoop()
{
    char InputBuffer[512] = {0};

    const char* TmpStr = "\n$";
    asm("int $0x80" :: "S"(0), "ebx"(TmpStr));

    unsigned char IsDone;
    Ps2::SetPs2InputBuffer((InputBuffer+1), 500, &IsDone);
        

        //Console::PrintInt((unsigned int)OnInnerAppExit, 1);


    while(!IsDone);

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

    while(1)
    {
        unsigned int AppClosed;
        asm("int $0x80" : "=ebx"(AppClosed): "S"(0x12), "ebx"(Output));
        if(AppClosed == 2){break;}
    }

    const char* TestPrint = "The App Has Closed\n";
    asm("int $0x80" :: "S"(0), "ebx"(TestPrint));

}


void CommandLine::CommandLineEntry()
{
    //Console::PrintString("Starting Background Watchdog Thread...\n");
    //Console::PrintString("Started Thread\n");
    asm("int $0x80" :: "S"(0x08));

    char* PrintString = "\nWelcome to Jenny Os Command Line v 0.0 (Very unstable)";
    asm("int $0x80" :: "S"(0), "ebx"(PrintString));
    unsigned int NumberAppsRan  = 0;
    
    for(;;)
    {
        CommandLoop();
    }

}
