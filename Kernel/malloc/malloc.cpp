#include "malloc.hpp"
#include "../console/console.hpp"
#include "../SpinLock.hpp"
#define MallocTableStart (0x200000) // 2 mb in memory
#define MaxNumberTables 5000

extern "C" void CopyMallocFunc();

struct Kmalloc::MallocTable* MallocTables = (struct Kmalloc::MallocTable*) MallocTableStart;

unsigned int NextFreeMalloc = MallocTableStart+(sizeof(struct Kmalloc::MallocTable)*MaxNumberTables); 
unsigned int NextFreeID;

unsigned char DebugMode = 0;
unsigned int MallocSpinLock = 0;

unsigned int NumberOverWrites =0;

unsigned int NumberBytesRam = 1*1024*1024*1024; // 1 gib

extern "C" void* CMalloc(unsigned int NumberBytes)
{
    char* TmpArea = (char*)Kmalloc::Malloc(NumberBytes);
    
    for(int i=0; i< NumberBytes; i++)
    {
        TmpArea[i] = 0;
    }

    return TmpArea; 
}
extern "C" void CFree(void* MemoryAddr){ Kmalloc::Free(MemoryAddr); }

struct Kmalloc::MallocTable* Kmalloc::GetTableCloseist(struct Kmalloc::MallocTable* StartTable, unsigned int MaxDistence)
{
    struct MallocTable* BestTable=0;
    for(int i=0; i< MaxNumberTables; i++)
    {
        unsigned int Diff;
        unsigned int LowAddr=0;
        unsigned int HighAddr = 0;
        unsigned int LowSize =0;
        if((unsigned int)(StartTable->MemoryAddr) > (unsigned int)(MallocTables[i].MemoryAddr))
        {
            Diff = (unsigned int)(StartTable->MemoryAddr) - (unsigned int)(MallocTables[i].MemoryAddr);
            LowAddr = (unsigned int)MallocTables[i].MemoryAddr;
            HighAddr = (unsigned int)StartTable->MemoryAddr;
            LowSize = (unsigned int)MallocTables[i].SizeBytes;
        }
        else
        {
            Diff = (unsigned int)(MallocTables[i].MemoryAddr) - (unsigned int )(StartTable->MemoryAddr);
            LowAddr = (unsigned int)(StartTable->MemoryAddr);
            HighAddr = (unsigned int)(MallocTables[i].MemoryAddr);
            LowSize = (unsigned int)(StartTable->SizeBytes);
        }
        if(LowAddr + LowSize == HighAddr && (MallocTables[i].Flags &1)){ return &MallocTables[i]; }
    }
    //Console::PrintInt(BestDiff, 0);
    return BestTable;
}

struct Kmalloc::MallocTable* Kmalloc::GetNextFreeTable()
{
    for(int i=0; i< MaxNumberTables; i++){
        if(MallocTables[i].MemoryAddr == 0)
        {
            return &MallocTables[i];
        }
    }
    return 0;
}

struct Kmalloc::MallocTable* Kmalloc::GetTableOfSize(unsigned int Size)
{
    for(int i=0; i< MaxNumberTables; i++)
    {
        if(MallocTables[i].SizeBytes >= Size && MallocTables[i].Flags & 1)
        {
            return &MallocTables[i];
        }
        /*if((MallocTables[i].SizeBytes - MallocTables[i].InUse >= Size) && (MallocTables[i].InUse < MallocTables[i].SizeBytes))
        {
            //Console::PrintInt(MallocTables[i].SizeBytes, 0);
            return &MallocTables[i];
        }*/
    }
    return 0;
}

struct Kmalloc::MallocTable* Kmalloc::GetTableOfAddr(void* Addr)
{
    for(int i=0; i< MaxNumberTables; i++)
    {
        if(MallocTables[i].MemoryAddr == Addr)
        {
            return &MallocTables[i];
        }
    }
    return 0;
}

void Kmalloc::ClearMallocTable(struct Kmalloc::MallocTable* Table )
{
    Table->Flags = 0;
    Table->MemoryAddr = (void*)0;
    Table->SizeBytes = 0;
    Table->InUse = 0;
}


void Kmalloc::Free(void* Addr)
{
    Spinlock::WaitSet(&MallocSpinLock);
    struct MallocTable* TmpTable = GetTableOfAddr(Addr);
    if(TmpTable == 0)
    {
        if(DebugMode) { Console::PrintString("Attempted to Free An Area that wasnt malloced\n"); }
        MallocSpinLock = 0;
        return;
    }
    if(TmpTable->Flags & (1 << 2)){ Console::PrintString("Skiping Chunk\n"); MallocSpinLock = 0; return; }
    if(TmpTable->Flags & (1 << 3)) { if(DebugMode){Console::PrintString("Removing remnet from defrag\n");} ClearMallocTable(TmpTable); MallocSpinLock = 0; return;}
    if(TmpTable->Flags & 1)
    {
        if(DebugMode) { Console::PrintString("Attempted to Free Already Freed Area\n"); }
        MallocSpinLock = 0;
        return;
    }
    TmpTable->Flags |= 1; // enable Free bit
    
    if (DebugMode)
    {    
        Console::PrintString("Freed Area ");
        Console::PrintInt((unsigned int)TmpTable->MemoryAddr, 1);
        Console::PrintString("\n");
    }
    
    unsigned int NumberTablesFound = 0;
    
    struct MallocTable* ClosistTable = GetTableCloseist(TmpTable, TmpTable->StartingSize);
    if(ClosistTable == 0){ MallocSpinLock = 0;return;}
    unsigned char IsLower = 0;
    if((unsigned int)TmpTable->MemoryAddr < (unsigned int) ClosistTable->MemoryAddr)
    {
        IsLower = 1;
    }

    struct MallocTable* LowistTable = (IsLower)?TmpTable:ClosistTable;
    struct MallocTable* HighestTable = (IsLower)?ClosistTable:TmpTable;

    LowistTable->SizeBytes += HighestTable->SizeBytes;
    if(DebugMode){Console::PrintString("Combined Area! To Size:");
    Console::PrintInt(LowistTable->SizeBytes, 0);
    Console::PrintString("\n");
    }
    ClearMallocTable(HighestTable);
    MallocSpinLock = 0;
}
#include "../io.hpp"

void Kmalloc::CheckIfOverlap()
{

    for(int i=0; i< NumberOverWrites; i++)
        {
            if (MallocOverWrites[i].MemoryAddress <= NextFreeMalloc && NextFreeMalloc <= (MallocOverWrites[i].MemoryAddress+MallocOverWrites[i].NumberBytes))
            {
                Console::PrintString("Hit Malloc Overwrite On Address:");
                Console::PrintInt(NextFreeMalloc, 1);
                Console::PrintString("\n");

                Console::PrintString("Moving Down To Address:");
                NextFreeMalloc = MallocOverWrites[i].MemoryAddress+MallocOverWrites[i].NumberBytes+1;
                Console::PrintInt(NextFreeMalloc, 1);
                Console::PrintString("\n");
                //for(int i=0; i< 0xFFFFFFFF; i++)
                //{
                //    IO::iowait();
                //}
                CheckIfOverlap();
            }
        }

    

}

void* Kmalloc::Malloc(unsigned int NumberBytes)
{
    /*if(MallocSpinLock)
    {
        Console::PrintString("Waiting For SpinLock(");
        Console::PrintInt(MallocSpinLock, 0);
        Console::PrintString(")");
    }*/
    Spinlock::WaitSet(&MallocSpinLock);
    //if(DebugMode){ Console::PrintString("Starting Malloc of "); Console::PrintInt(NumberBytes, 0); Console::PrintString(" Bytes\n"); }
   
    struct MallocTable* TableReuseing = GetTableOfSize(NumberBytes);
    if(TableReuseing == 0)
    {
        if(DebugMode) { Console::PrintString("Allocing New Area in Memory!\n"); }
        struct MallocTable* TableUsing = GetNextFreeTable();
        if(TableUsing == 0) { Console::PrintString("Malloc Tables Full Error\n") ; MallocSpinLock = 0;return 0; }
        
        
        CheckIfOverlap();

        
        TableUsing->MemoryAddr = (void*) NextFreeMalloc;
        


        
        NextFreeMalloc += NumberBytes;
        if((unsigned int)TableUsing->MemoryAddr > NextFreeMalloc || NextFreeMalloc >= NumberBytesRam && NumberBytesRam != 0)
        {
            Console::PrintString("Out Of Memory Error\n");
            while(1);
        }
        TableUsing->SizeBytes = NumberBytes;
        TableUsing->StartingSize = NumberBytes;
        TableUsing->InUse = NumberBytes;
        if(DebugMode) { Console::PrintString("Alloced Area "); Console::PrintInt((unsigned int)TableUsing->MemoryAddr, 1); Console::PrintString("\n"); }
        MallocSpinLock = 0;
        return TableUsing->MemoryAddr;
    }
    if(!(TableReuseing->Flags & 1))
    {
        if(DebugMode){Console::PrintString("Using Unused chunk of header\n");}
        struct MallocTable* SpiltTable = GetNextFreeTable();
        if(SpiltTable == 0) { MallocSpinLock = 0; return 0; }
        SpiltTable->Flags = 0;
        SpiltTable->MemoryAddr= (void*) ((unsigned int)TableReuseing->MemoryAddr+TableReuseing->InUse);
        SpiltTable->SizeBytes = TableReuseing->SizeBytes - TableReuseing->InUse;
        TableReuseing->SizeBytes = TableReuseing->InUse;
        MallocSpinLock = 0;
        return SpiltTable->MemoryAddr;
    }
    if(DebugMode){ Console::PrintString("Reusing Area!\n"); }
    

    if(NumberBytes != TableReuseing->SizeBytes)
    {
        struct MallocTable* SpiltTable = GetNextFreeTable();
        if(SpiltTable == 0){  Console::PrintString("Malloc Table Full Error\n") ;MallocSpinLock = 0;return 0; }
        SpiltTable->Flags = 3; // sets free and Islower                       //
        SpiltTable->SizeBytes = TableReuseing->SizeBytes-NumberBytes;
        SpiltTable->MemoryAddr = (void*)((unsigned int)TableReuseing->MemoryAddr+NumberBytes);
        TableReuseing->SizeBytes = NumberBytes;
        TableReuseing->Flags = 0;
        if(DebugMode){ Console::PrintString("Malloc Spilt Table Event\n"); } 
        TableReuseing->InUse = NumberBytes;
        MallocSpinLock = 0;
        return TableReuseing->MemoryAddr;
    }
    else
    {
        TableReuseing->Flags &= ~(1); // uncheck free bit
        TableReuseing->InUse = NumberBytes;
        MallocSpinLock = 0;
        return TableReuseing->MemoryAddr;
    }
    MallocSpinLock = 0;
    return 0;
}

struct MallocOverWrite* Kmalloc::MallocOverWrites;


void Kmalloc::AddOverwrite(struct MallocOverWrite Table)
{
    struct MallocOverWrite* TmpTable;
    if(NumberOverWrites == 0)
    {
        TmpTable = (struct MallocOverWrite*) Kmalloc::Malloc(sizeof(struct MallocOverWrite));
    }
    else
    {
        TmpTable = (struct MallocOverWrite*) Kmalloc::Malloc(sizeof(struct MallocOverWrite) * (NumberOverWrites+1));
    }

    for(int i=0; i< NumberOverWrites; i++)
    {
        TmpTable[i] = MallocOverWrites[i];
    }
    TmpTable[NumberOverWrites] = Table;


    NumberOverWrites++;
    Console::PrintString("Added Malloc Overwrite:");
    Console::PrintInt(Table.MemoryAddress, 1);
    Console::PrintString("|");
    Console::PrintInt(Table.NumberBytes, 1);
    Console::PrintString("\n");
    Kmalloc::Free(MallocOverWrites);
    MallocOverWrites = TmpTable;

    

}

extern "C" void MallocMemoryMapEntry();

void Kmalloc::MallocInit()
{
    NumberBytesRam = 0; // one GIB
    MallocSpinLock = 0;
    MallocOverWrites = 0;
    NumberOverWrites = 0;
    // reset the vars just in case they got messed up
    MallocTables = (struct MallocTable*) MallocTableStart;
    NextFreeMalloc = MallocTableStart+(sizeof(struct MallocTable)*MaxNumberTables);
    DebugMode = 0;

    

    Console::PrintString("Malloc System Init\n");
    for(int i=0; i< MaxNumberTables; i++)
    {

        struct MallocTable Tmp = {0};
        MallocTables[i] = Tmp;
    }
    NextFreeID = 1;
    Console::PrintString("Cleared Malloc Table Area\n"); 

    CopyMallocFunc();
    for(int i=0; i< 512; i++)
    {
        ((char*)0x8000)[i] = 0;
    }

    MallocMemoryMapEntry();

    Console::PrintString("Maps Stored in 0x8000, parsing...\n");
    unsigned int MapAddress = 0x8004;
    while(1)
    {
        
        struct MapStruct 
        {
            unsigned long long Address;
            unsigned long long SizeBytes;
            unsigned int Type;
            unsigned int AcpiBits;
        };

        struct MapStruct* MapPtr = (struct MapStruct*)MapAddress;
        if((MapPtr->Address & 0xFFFFFFFF) != 0 && (MapPtr->SizeBytes & 0xFFFFFFFF) != 0)
        {

        
            if(MapPtr->Type != 1 && MapPtr->Type != 3) // this is all blocks that are marked as bad, ACPI needed or resurcved by system
            {
                Console::PrintString("Blocked Area Found(");
                Console::PrintInt(MapPtr->Address & 0xFFFFFFFF, 1);
                Console::PrintString(", ");
                Console::PrintInt(MapPtr->SizeBytes & 0xFFFFFFFF, 1);
                Console::PrintString(")\n");
                struct MallocOverWrite Table;
                Table.MemoryAddress = (MapPtr->Address & 0xFFFFFFFF);
                Table.NumberBytes = (MapPtr->SizeBytes & 0xFFFFFFFF);
                AddOverwrite(Table);
            }
            else
            {
                Console::PrintString("Primary Usable Area Stated(");
                Console::PrintInt(MapPtr->Address & 0xFFFFFFFF, 1);
                Console::PrintString(", ");
                Console::PrintInt(MapPtr->SizeBytes & 0xFFFFFFFF, 1);
                Console::PrintString(")\n");
                if(NumberBytesRam < ((MapPtr->SizeBytes & 0xFFFFFFFF) + (MapPtr->Address & 0xFFFFFFFF))){
                    NumberBytesRam = ((MapPtr->SizeBytes & 0xFFFFFFFF) + (MapPtr->Address & 0xFFFFFFFF));
                }
            }
        }
        MapAddress += 24;
        if(MapAddress > (0x8000 +512)){ break;}
    }
    Console::PrintString("Memory Size Stated:");
    Console::PrintInt((NumberBytesRam/1024/1024), 0);
    Console::PrintString(" Mib\n");
    Console::PrintString("Done\n");

    if((NumberBytesRam/1024/1024) <= 1022)
    {
        Console::PrintString("you do not have enough memory to run jenny os\n");
        asm("cli\nhlt");
    } 
    //asm("cli\nhlt");



#define NumberTests 32
    Console::PrintString("Starting 512 byte test, wait a sec\n");
    void* OtherTestArea = Malloc(256);
    void* Main512Chunk = Malloc(16*NumberTests);
    Free(Main512Chunk);

    void* SmallChunks[NumberTests];
    for(int i=0; i< NumberTests; i++)
    {
        SmallChunks[i] = Malloc(16);
    }
    for(int i = (NumberTests-1); i >= 0; i--)
    {
        Free(SmallChunks[i]);
    }
    for(int i=0; i< NumberTests; i++)
    {
        SmallChunks[i] = Malloc(16);
    }
    for(int i=0; i< NumberTests; i++)
    {
        Free(SmallChunks[i]);
    }
    void* TestArea =Malloc(16);
    void* TestAllocSpace = Malloc(10);
    
    if(TestArea == TestAllocSpace)
    {
        Console::PrintString("Malloc OverLap Error\n");
    }

    Free(TestAllocSpace);
    Free(TestArea);
    Free(OtherTestArea);
    Console::PrintString("Test done\n");
}
