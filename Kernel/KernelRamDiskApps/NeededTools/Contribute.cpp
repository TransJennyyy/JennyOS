#include "Contribute.hpp"

const char* StartMsg = "So you want to either report a bug or help with the OS?\nWell report bugs to my email at transjennyyyyy@gmail.com\nIf you want to contribute to the OS first clone the main repo at github.com/transjennyyy/JennyOS.git. Use the read me for how to compile and check the ISSUES.md for known issues. Once you've added whatever you want to add to the project send your git patch file(preferably using --stdout) to my email(same as before)\n";


void Contribute::Entry()
{
    asm("int $0x80" :: "S"(0), "ebx"(StartMsg));
    asm("int $0x80" :: "S"(0x30));
}
