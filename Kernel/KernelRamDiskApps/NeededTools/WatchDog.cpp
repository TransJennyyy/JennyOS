#include "WatchDog.hpp"
#include "../../console/console.hpp"

unsigned char WatchDog::Started;
void WatchDog::Entry()
{
    //Console::PrintString("Watch Dog Started\n");
    Started = 1;

    while(1)
    {
        (*(char*)(0xb8000+158))++;
    }
}