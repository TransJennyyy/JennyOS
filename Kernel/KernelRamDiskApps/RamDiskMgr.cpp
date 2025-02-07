#include "RamDiskMgr.hpp"
#include "CommandLine/CommandLine.hpp"
#include "NeededTools/WatchDog.hpp"
#include "../FS/FileSystem.hpp"
#include "../console/console.hpp"
#include "NeededTools/tasklist/tasklist.hpp"
#include "NeededTools/help/help.hpp"
#include "Why/why.hpp"
char* RamDiskMgr::OverrideArray;
void* RamDiskMgr::OverrideAddr[NumberOverides];

void RamDiskMgr::init()
{
    OverrideArray = (char*)"/CommandLine.jexe\0/WatchDog.jexe\0/tl.jexe\0/help.jexe\0"; 
    OverrideAddr[0] = (void*) CommandLine::CommandLineEntry;
    OverrideAddr[1] = (void*) WatchDog::Entry;
    OverrideAddr[2] = (void*) TaskList::Entry;
    OverrideAddr[3] = (void*) Help::Entry;
}

void* RamDiskMgr::ReadFile(const char* Path)
{
    char* OverrideString = OverrideArray;
    unsigned int OverideIndex = 0xFFFFFFFF;
    for(int i=0; i< NumberOverides; i++)
    {
        //Console::PrintString("Checking Path:");
        //Console::PrintString(OverrideString);
        //Console::PrintString("\n");
        unsigned int PathSize = FS::GetFilePathSize((void*)Path); 
        unsigned int OverideSize = FS::GetFilePathSize((void*)OverrideString);

        if(PathSize != OverideSize)
        {
            OverrideString += OverideSize+1;
            continue;
        }
        unsigned char WasFound = 1;
        for(int j=0; j< PathSize; j++)
        {
            if(Path[j] != OverrideString[j]){ WasFound =0;break;}
        }
        if(!WasFound){continue;}
        OverideIndex = i;
        break;
    }
    if(OverideIndex == 0xFFFFFFFF){
        Console::PrintString("No Disk Override\n");

        unsigned long long FileLBA = FS::FindFileLBA(Path);
        if(FileLBA == 0)
        {
            Console::PrintString("No Such File\n");
            return 0;
        }

    
        return FS::ReadFileFromLBA(FileLBA);
    }



    //Console::PrintString("Giving Address:");
    //Console::PrintInt((unsigned int)OverrideAddr[OverideIndex], 1);
    //Console::PrintString("\n");

    return OverrideAddr[OverideIndex];
    
}
