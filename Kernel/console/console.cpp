#include "console.hpp"
#include "../io.hpp"
#include "../SpinLock.hpp"
unsigned int Console::CurrentOffset;
char* Console::VGAbuffer;
unsigned int Console::VgaSpinLock;
void Console::VGAInit()
{
    CurrentOffset = 0;
    VgaSpinLock = 1; // prevent prints from happening while the screen is being cleared/setup
    VGAbuffer = (char*) 0xb8000;
    for(int i=0; i< 80*25*2; i+=2)
    {
        VGAbuffer[i] = ' ';
    }
    VgaSpinLock = 0;
}


void Console::ScrollScreen()
{
    for(int i=160; i< 80*25*2; i++)
    {
        VGAbuffer[i-160] = VGAbuffer[i];
        VGAbuffer[i-159] = 0;
    }
    CurrentOffset -= 160;
}

void Console::NewLine()
{
    CurrentOffset /= 160;
    CurrentOffset+=1;
    CurrentOffset *= 160;
}

void Console::PrintString(const char* string)
{
    Spinlock::WaitSet(&VgaSpinLock);
    
    int CurrentIndex = 0;
    while(1)
    {
        unsigned char IsPrinting = 1;
        switch(string[CurrentIndex])
        {
            case '\n':
                NewLine();
                IsPrinting = 0;
                break;
            default:
                break;
        }
            if(IsPrinting){
                VGAbuffer[CurrentOffset] = string[CurrentIndex];
                CurrentOffset+=2;
        }
         
        CurrentIndex++;
        if(CurrentOffset >= 80*24*2){ ScrollScreen(); }
        if(string[CurrentIndex] == 0){break;}
    }

    IO::outb(0x3D4, 0x0f);
    IO::outb(0x3D5, (unsigned char)((CurrentOffset/2) & 0xff));
    IO::outb(0x3D4, 0x0e);
    IO::outb(0x3D5, (unsigned char)(((CurrentOffset/2)>>8)&0xff));
    VgaSpinLock = 0;
    asm("sti");

}

void Console::PrintString(const char* string, unsigned char InISR)
{
    Spinlock::WaitSet(&VgaSpinLock);

    int CurrentIndex = 0;
    while(1)
    {
        unsigned char IsPrinting = 1;
        switch(string[CurrentIndex])
        {
            case '\n':
                NewLine();
                IsPrinting = 0;
                break;
            default:
                break;
        }
            if(IsPrinting){
                VGAbuffer[CurrentOffset] = string[CurrentIndex];
                CurrentOffset+=2;
        }
         
        CurrentIndex++;
        if(CurrentOffset >= 80*24*2){ ScrollScreen(); }
        if(string[CurrentIndex] == 0){break;}
    }

    IO::outb(0x3D4, 0x0f);
    IO::outb(0x3D5, (unsigned char)((CurrentOffset/2) & 0xff));
    IO::outb(0x3D4, 0x0e);
    IO::outb(0x3D5, (unsigned char)(((CurrentOffset/2)>>8)&0xff));
    VgaSpinLock = 0;

}


void Console::PrintInt(unsigned int Number, unsigned char IsHex)
{
    //PrintString("Printing String\n");
    unsigned int NumberLetters  = 0;
    char PrintBuffer[50] = {0};
    PrintBuffer[0] = '0';
    unsigned int TmpNumber = Number;
    while(TmpNumber !=0){ TmpNumber = IsHex?TmpNumber/16:TmpNumber/10; NumberLetters++; }
    if((NumberLetters+1) >= 50)
    {
        PrintString("Overflow Error: int too big to print\n");
    }
    if(IsHex){PrintBuffer[NumberLetters] = 'h';}
    if(!IsHex)
    {
    
        while(Number!=0)
        {
            unsigned char LetterPrinting = (unsigned char)((Number % 10) & 0xff);
            Number/=10;

            LetterPrinting += '0';
            PrintBuffer[NumberLetters-1] = LetterPrinting;
            NumberLetters--;
            if(Number == 0){break;} // sometimes the compiler breaks the while loop so this is to fix it

        }
        
    }
    else
    {
        
        while(Number!=0)
        {
            unsigned char LetterPrinting = (unsigned char)((Number % 16) & 0xff);
            Number/=16;

            if(LetterPrinting < 10){
                LetterPrinting += '0';
            }
            else
            {
                LetterPrinting -= 10;
                LetterPrinting+= 'A';
            }
            PrintBuffer[NumberLetters-1] = LetterPrinting;
            NumberLetters--;
            if(Number == 0){break;} // sometimes the compiler breaks the while loop so this is to fix it

        }
    }
    PrintString(PrintBuffer);
}
