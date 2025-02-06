#include "io.hpp"

void IO::iowait()
{
    outb(0x80, 0);
}

void IO::outb(unsigned short IoAddr, unsigned char data)
{
    asm("outb %1, %0" : : "d"(IoAddr), "a"(data));
}


void IO::outs(unsigned short IoAddr, unsigned short data)
{
    asm("outw %1, %0" : : "d"(IoAddr), "a"(data));
}

void IO::outl(unsigned short IoAddr, unsigned int data)
{
    asm("outl %1, %0" :: "d"(IoAddr), "a"(data));
}

unsigned int IO::inl(unsigned short IoAddr)
{
    unsigned int OutVar = 0;
    asm("inl %1, %0" : "=a"(OutVar) : "d"(IoAddr) );
    return OutVar;
}


unsigned short IO::ins(unsigned short IoAddr)
{
    unsigned short OutVar = 0;
    asm("inw %1, %0" : "=a"(OutVar) : "d"(IoAddr) );
    return OutVar;
}

unsigned char IO::inb(unsigned short IoAddr)
{
    unsigned char OutVar = 0;
    asm("inb %1, %0" : "=a"(OutVar) : "d"(IoAddr));
    return OutVar;
}
