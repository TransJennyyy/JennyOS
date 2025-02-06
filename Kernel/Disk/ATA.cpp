#include "ATA.hpp"
#include "../io.hpp"
#include "../console/console.hpp"


unsigned char ATA::CheckIfUsable()
{
    Console::PrintString("Checking if ATA is usable main disk driver...");

    if(IO::inb(ATAMainBus+7) == 0xff)
    {
        Console::PrintString("Unusable\n");
        return 0; // floating bus error
    }

    Console::PrintString("Usable(No Driver)\n");
    // TODO add ATA driver
    return 0;



}
