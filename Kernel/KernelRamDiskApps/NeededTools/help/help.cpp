#include "help.hpp"

const char* HelpString = "Welcome To Jenny's OS( An os made in 20 DAYS )\nCommandList\nhelp - this command | tl - list info about all open threads\n";

void Help::Entry()
{

    asm("int $0x80" :: "S"(0), "ebx"(HelpString));

    asm("int $0x80" :: "S"(0x30));
    while(1)
    {
        (*((char*)0xb8000+250))++;
    }

}