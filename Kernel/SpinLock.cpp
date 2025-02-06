#include "SpinLock.hpp"
#include "console/console.hpp"

extern "C" unsigned int SpinLockReset(unsigned int Check, unsigned int* SpinLockVar);

void Spinlock::WaitSet(unsigned int* SpinLockVar )
{
    //Console::PrintString("Waiting For SpinLock:");
    //Console::PrintInt(*SpinLockVar, 0);
    //Console::PrintString("\n");
    while(1)
    {
        unsigned int Check = 1;
        Check = SpinLockReset(Check, SpinLockVar);


        if(!Check)
        {
            // in such case we are allowed to leave as the spinlock is now ours
            //Console::PrintString("Spin Lock Ended\n");
            return;
        }
    }

}