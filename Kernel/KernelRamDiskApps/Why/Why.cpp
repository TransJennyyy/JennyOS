#include "why.hpp"
#include "../../io.hpp"
void why::Entry()
{

    asm("int $0x80" :: "S"(0x08));

    const char* FirstFrame = "\n\nThis is a cool os i made\nSorry i was gone so long\n";
    asm("int $0x80" :: "S"(0), "ebx"(FirstFrame));
    for(int i=0; i< 0xFFFFFF; i++){ IO::iowait(); }
    for(int i=0; i< 0xFFF; i++){ IO::iowait(); }

    const char* SecondFrame = "Sorry but Ill be long gone\n";
    asm("int $0x80" :: "S"(0), "ebx"(SecondFrame));
    for(int i=0; i< 0xFFFFFF; i++){ IO::iowait(); }
    for(int i=0; i< 0xFFF; i++){ IO::iowait(); }

    const char* FinnalFrame = "By the time this video is up... \nBut I love each one of you who has watched me in the past... Goodbye... forever\n";
    asm("int $0x80" :: "S"(0), "ebx"(FinnalFrame));


    while(1);

}