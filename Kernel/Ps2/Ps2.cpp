#include "Ps2.hpp"
#include "../idt.hpp"
#include "../io.hpp"
#include "../console/console.hpp"
#include "../Exec/Thread.hpp"
#include "../Cpuid.hpp"
unsigned char ShiftDown;

char* CurrentBuffer;
unsigned int BufferSizeBytes;
unsigned char BufferDone;
unsigned int BufferIndex;
extern unsigned int NumberCores;
extern unsigned char* CoreIDS;
extern unsigned int* CoreCurrentThreadID;

Ps2ReadDoneFuncType Ps2::Ps2ReadDone;
struct ThreadObject* ThreadGetingInput;
extern struct ThreadObject* ThreadArray;
unsigned char* IsDonePtr;

void Ps2::SetPs2InputBuffer(void* Buffer, unsigned int NumberBytes, void* ThreadObj)
{
    CurrentBuffer = (char*)Buffer;
    BufferSizeBytes = NumberBytes;
    BufferDone = 0;
    BufferIndex = 0;
    ThreadGetingInput = (struct ThreadObject*) ThreadObj;
    ThreadGetingInput->ThreadWaitTime = 0; // make the thread waits until the Ps2 system unpauses it
    ThreadGetingInput->ThreadState = ThreadStatePaused;
    IsDonePtr = 0;
}

void Ps2::SetPs2InputBuffer(void* Buffer, unsigned int NumberBytes, unsigned char* IsDone)
{
    *IsDone = 0;
    IsDonePtr = IsDone;
    CurrentBuffer = (char*)Buffer;
    BufferSizeBytes = NumberBytes;
    BufferDone = 0;
    BufferIndex = 0;

}


void Ps2::Init()
{
    ShiftDown = 0;
    CurrentBuffer = 0;
    BufferSizeBytes = 0;
    BufferDone = 1;
    Ps2ReadDone = 0;
    BufferIndex = 0;
    ThreadGetingInput = 0;
    IsDonePtr = 0;
}
unsigned char Ps2::Ps2KeyPressed;
void Ps2::OnPs2CheckCycle() // TODO: switch from polling to IRQ's, LOWIST priority
{
    Ps2KeyPressed = 0;
    if(IO::inb(0x64) & 1)
    {
        
        unsigned char Ps2KeyCode[256] = {0,0,'1', '2', '3', '4', '5', '6','7', '8', '9','0', '-', '=',
        0,0,'q','w','e','r','t','y','u','i','o','p', '[', ']',0,0,'a','s', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
        ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',0,0,0,' ' ,0};
        unsigned char Ps2KeyCodeShift[256] = {0,0,'!', '@', '#', '$', '%', '^','&', '*', '(',')', '_', '+',
        0,0,'Q','W','E','R','T','Y','U','I','O','P', '{', '}',0,0,'A','S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
        ':', '"', '`', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',0,0,0,' ' ,0};
        //Console::PrintString("Input Found\n");
        unsigned char Ps2Letter = IO::inb(0x60);

        switch (Ps2Letter)
        {
        case 0x2A:
            ShiftDown = 1;
            break;
        case 0xAA:
            ShiftDown = 0;
            break;
        case 0x1c:
            if(CurrentBuffer == 0){ break; }
            CurrentBuffer[BufferIndex] = 0;
            BufferDone =1;
            if(Ps2ReadDone != 0)
            {
                Ps2ReadDone();
                Ps2ReadDone = 0;
            }
            if(IsDonePtr != 0){
                *IsDonePtr = 1;
                IsDonePtr = 0;
            }
            BufferIndex = 0;
            CurrentBuffer = 0;
            if(ThreadGetingInput != 0)
            {
                ThreadGetingInput->ThreadState = ThreadStateRestarting;
            }
            break;
        
        default:
            break;
        }

        //Console::PrintInt(Ps2Letter, 0);
        //Console::PrintString("\n");
        if(BufferSizeBytes > BufferIndex && Ps2KeyCode[Ps2Letter] != 0 && CurrentBuffer != 0){ // prevents a buffer overrun
            CurrentBuffer[BufferIndex] = (ShiftDown)? Ps2KeyCodeShift[Ps2Letter]:Ps2KeyCode[Ps2Letter];
            BufferIndex++;
        }
        /*else
        {
            Console::PrintString("Buffer Full\n");
        }*/

        if(Ps2KeyCode[Ps2Letter] != 0){
            Ps2KeyPressed = 1;
            char AsciiChar = (ShiftDown)? Ps2KeyCodeShift[Ps2Letter]:Ps2KeyCode[Ps2Letter];
            char LetterStr[2] = {AsciiChar, 0};
            Console::PrintString(LetterStr);
        }

    }
}
