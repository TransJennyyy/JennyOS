#include "../console/console.hpp"
#include "../Apic/Apic.hpp"
#include "../io.hpp"

extern "C" void DiskReadFunc(unsigned int Sector);
extern "C" void DiskWriteFunc(unsigned int Sector);
extern "C" void CopyWriteFunc();
extern "C" void BackUpSystemInit();

void DiskReadFuncWrapper(unsigned int WantedAddr, unsigned int NumberSectors, unsigned int Sector)
{
    BackUpSystemInit();
    //Console::PrintString("Starting Read\n");
    Apic::WriteLAPICReg(0xf0, 0xff);
    asm("cli");
    double PercentDone = 0;
    unsigned int LastPercent = 0;

    if(NumberSectors > 50)
    {
        Console::PrintString("\nWarning Large Read On Backup Driver, Will Show Loading Bar\n");
    }



    for(int SectorOffset =0 ; SectorOffset < NumberSectors; SectorOffset++){

        

        DiskReadFunc(Sector+SectorOffset);
        
        unsigned int CurrentIndex = 0;
        for(int i=0x8000; i< 0x8000+512; i++)
        {
            ((char*)WantedAddr)[CurrentIndex] =  *((char*)i);
            CurrentIndex++;
        }
        WantedAddr += 512;

        PercentDone = ((double)SectorOffset / (double)NumberSectors) * 80;
        if(LastPercent != ((unsigned int)PercentDone))
        {
            LastPercent = (unsigned int)PercentDone;
            Console::PrintString("#");
        }
        

    }

    if(NumberSectors > 50)
    {
        Console::PrintString("\n");
    }
    
    asm("cli");
    //Apic::WriteLAPICReg(0xb0, 0);
    //Apic::WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
    //Apic::WriteLAPICReg(0x3E0, 0x3);
    //Apic::WriteLAPICReg(0x380,(Apic::ReadLAPICReg(0x380)));
    //Apic::WriteLAPICReg(0xf0, 0x100); // reset the core timer
    //asm("sti");

    //Console::PrintString("Done Reading\n");


}

void DiskWriteFuncWrapper(unsigned int WantedAddr, unsigned int NumberSectors, unsigned int Sector)
{
    CopyWriteFunc();

    for(int i=0; i < NumberSectors; i++)
    {
        for(int SecSize =0; SecSize < 512; SecSize++)
        {
            ((char*)0x8000)[SecSize] = ((char*)WantedAddr)[SecSize];
        }
        DiskWriteFunc(Sector+i); // this goes into 16 bit mode to use the bios
        WantedAddr+=512;
        Sector++;
    }

   
    // we need to reset the LAPIC timers after unless we want the BSC core to stop working after every read/write of the disk
    Apic::WriteLAPICReg(0xb0, 0);
    Apic::WriteLAPICReg(0x320, (0x32 | (1 << 17) ));
    Apic::WriteLAPICReg(0x3E0, 0x3);
    Apic::WriteLAPICReg(0x380,(Apic::ReadLAPICReg(0x380)));
    Apic::WriteLAPICReg(0xf0, 0x100); // reset the core timer
    asm("sti");
    
}