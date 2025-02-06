#include "FileSystem.hpp"
#include "../console/console.hpp"
#include "../PCI/PCI.hpp"
#include "../malloc/malloc.hpp"
#include "../io.hpp"
#include "../Paging/Paging.hpp"
#define DefaultStartLBA 256+2


unsigned int FS::FileSystemOffset;
struct FsHeader* FS::FileSystemHeader;
// this will ret the LBA's to the INDEX's not the files
unsigned long long* FS::ScanDir(const char* Path, unsigned int* NumberFilesFoundRet) // Files Will Kinda work like unix, eg: /Folder/AnotherFoler/File.txt
{
    Console::PrintString("Scaning Path:");
    Console::PrintString(Path);
    Console::PrintString("\n");

    unsigned char ScaningRoot = 0;
    unsigned int PathScanSize= 0;
    if(Path == "/")
    {
        ScaningRoot = 1;
    }
    while(Path[PathScanSize] != 0){ PathScanSize++; }
    unsigned int NumberFilesFound = 0;
    char FileHeader[512] = {0};
    if(FileHeader == 0)
    {
        Console::PrintString("Error Cant Alloc memory\n");
        asm("cli\nhlt");
    }
    unsigned long long CurrentLBA = FileSystemOffset+1;
    while(1)
    {
        PCI::BootDiskRead((unsigned int)FileHeader, 1, CurrentLBA);
        unsigned int PathSize = 0;
        while(FileHeader[PathSize] != 0){ PathSize++; }
        unsigned char IsFileLookingFor = 1;
        //Console::PrintString(FileHeader);
        //Console::PrintString("\n");
        if(!ScaningRoot)
        {
            // here we check the path, if the start matchs we keep it otherwise find the next index LBA
            for(int i=1; i< PathScanSize; i++) // we skip the / as its implyed in the fs
            {
                if (FileHeader[i-1] != Path[i])
                {
                    IsFileLookingFor = 0;
                }
            }
        }
        if(IsFileLookingFor){
            // Here we get Data About the Object
            unsigned char IsFolder = 1;
            for(int i=0; i< 10; i++){
                if((FileHeader[PathSize+1+i] & 0xff) != 0xff)
                {
                    IsFolder = 0;
                }
            }
            if(!IsFolder) 
            {
                //Console::PrintString("Found File...\n");
                NumberFilesFound++;
            }
        
        }
        // now we get the offset leading us to the next file in the system
        struct FileIndex* FilePtr = (struct FileIndex*)((unsigned int)FileHeader + PathSize+1);
        unsigned long long NextLBA = FilePtr->NextLBALow | ((unsigned long long)FilePtr ->NextLBAHigh << 32);
        CurrentLBA = (NextLBA);
        CurrentLBA &= 0xFFFFFFFFFFFF; // limit to 48 bits
        if(CurrentLBA == 0)
        {
            break;
        }
        CurrentLBA += FileSystemOffset+1;


    }
    /*Console::PrintString("Found ");
    Console::PrintInt(NumberFilesFound, 0);
    Console::PrintString(" Number Files\n");
*/
    *NumberFilesFoundRet = NumberFilesFound;
    unsigned long long* RetNumber = (unsigned long long*)Kmalloc::Malloc(NumberFilesFound*sizeof(unsigned long long));
    if(RetNumber == 0)
    {
        Console::PrintString("Memory Alloc Fail\n");
        return 0;
    }
    for(int i=0; i< NumberFilesFound; i++)
    {
        RetNumber[i] = 0;
    }
    NumberFilesFound = 0;
    CurrentLBA = FileSystemOffset+1;
    while(1)
    {
        /*Console::PrintString("Checking LBA:");
        Console::PrintInt(CurrentLBA, 0);
        Console::PrintString("\n");
        */
        PCI::BootDiskRead((unsigned int)FileHeader, 1, CurrentLBA);
        /*Console::PrintString(FileHeader);
        Console::PrintString("\n");
        */
        unsigned int PathSize = 0;
        while(FileHeader[PathSize] != 0){ PathSize++; }
        unsigned char IsFileLookingFor = 1;
        if(!ScaningRoot)
        {
            // here we check the path, if the start matchs we keep it otherwise find the next index LBA
            for(int i=1; i< PathScanSize; i++) // we skip the / as its implyed in the fs
            {
                if (FileHeader[i-1] != Path[i])
                {
                    IsFileLookingFor = 0;
                }
            }
        }
        if(IsFileLookingFor){
            // Here we get Data About the Object
            unsigned char IsFolder = 1;
            for(int i=0; i< 10; i++){
                if((FileHeader[PathSize+1+i] & 0xff) != 0xff)
                {
                    IsFolder = 0;
                }
            }
            if(!IsFolder) 
            {
                RetNumber[NumberFilesFound] = CurrentLBA;
                NumberFilesFound++;
                //Console::PrintString("Adding To Index\n");
            }
        
        }
        // now we get the offset leading us to the next file in the system
        struct FileIndex* FilePtr = (struct FileIndex*)((unsigned int)FileHeader + PathSize+1);
        unsigned long long NextLBA = (unsigned long long)FilePtr->NextLBALow | ((unsigned long long)FilePtr ->NextLBAHigh << 32);
        CurrentLBA = (NextLBA);
        CurrentLBA &= 0xFFFFFFFFFFFF; // limit to 48 bits
        if(CurrentLBA == 0)
        {
            break;
        }
        CurrentLBA += FileSystemOffset+1;
        



    }
    //Console::PrintString("Done, Reting\n");
    return RetNumber;

}

unsigned char FS::FileSystemSetup;
unsigned long long FS::LastIndexTable;
unsigned long long FS::NextFreeLBA;
void FS::init()
{   
    FileSystemSetup = 0;
    Console::PrintString("File system is being scaning for...\n");

    FileSystemHeader = (struct FsHeader*) Kmalloc::Malloc(512);
    

    PCI::BootDiskRead((unsigned int)FileSystemHeader, 1, 258);

    const char* FileSystemHeaderCmp = "JOSFS";
    for(int i=0; i< 5; i++)
    {
        if(FileSystemHeader->MagicBits[i] != FileSystemHeaderCmp[i])
        {
            Console::PrintString("ERROR: FS system header couldnt be found\n");
            asm("cli\nhlt");
        }
    }

    Console::PrintString("FS has been confermed\n");


    Console::PrintString("File System Stated Offset:");
    Console::PrintInt(FileSystemHeader->LbaOffset, 0);
    Console::PrintString("\n");
    Console::PrintString("File System Next Free LBA:");
    Console::PrintInt((FileSystemHeader->NextFreeLBA + FileSystemHeader->LbaOffset), 0);
    Console::PrintString("\n");

    FileSystemOffset = FileSystemHeader->LbaOffset;

    unsigned int NumberFilesFound = 0;
    unsigned long long* FileIndexLbas =  FS::ScanDir("/", &NumberFilesFound);
    if(FileIndexLbas == 0 || NumberFilesFound == 0)
    {
        Console::PrintString("Fs Grab File Test Failed\n");
        asm("cli\nhlt");
    }

    Console::PrintString("Root Scan Found ");
    Console::PrintInt(NumberFilesFound, 0);
    Console::PrintString(" Files\n");
    Console::PrintString("File Index LBA's:");
    for(int i=0; i<  NumberFilesFound; i++)
    {
        Console::PrintInt((unsigned int)FileIndexLbas[i], 0);
        Console::PrintString(",");
    }
    Console::PrintString("\n");
    Kmalloc::Free(FileIndexLbas);




}

unsigned int FS::FileSizeFromLBA(unsigned long long LBA)
{
    unsigned int FileSize = 0;

    LBA += FileSystemHeader->LbaOffset;

    char TmpHeader[512];
    PCI::BootDiskRead((unsigned int)TmpHeader, 1, LBA);

    Console::PrintString(TmpHeader);
    while(1);



}

unsigned long long* FS::GetFreedIndexTables(unsigned int* NumberFound)
{
    unsigned long long CurrentLBA = FileSystemHeader->LbaOffset+1;

    char* FileFound = (char*) Kmalloc::Malloc(512);

    unsigned int NumberFilesFree = 0;


    while(1)
    {
        PCI::BootDiskRead((unsigned int)FileFound, 1, (unsigned int)(CurrentLBA & 0xFFFFFFFF));

        unsigned int NameSize = GetFilePathSize(FileFound);
        struct FileIndex* FilePtr = (struct FileIndex*)((unsigned int)FileFound+NameSize+1);
        if(FilePtr->MetaData[0] & 1)
        {
            NumberFilesFree++;
        }

        CurrentLBA = FilePtr->NextLBALow | ((unsigned long long)FilePtr->NextLBAHigh << 32);
        if(CurrentLBA == 0)
        {
            break;
        }
        CurrentLBA += FileSystemHeader->LbaOffset+1;
    }

    Console::PrintString("Found ");
    Console::PrintInt(NumberFilesFree, 0);
    Console::PrintString(" Overwritable Index\\s\n");
    
    unsigned long long* RetNumbers = (unsigned long long*) Kmalloc::Malloc(sizeof(unsigned long long)*NumberFilesFree);
    if(RetNumbers == 0)
    {
        Console::PrintString("Alloc Error\n");
        Kmalloc::Free(FileFound);
        return 0;
    }

    NumberFilesFree = 0;
    CurrentLBA = FileSystemHeader->LbaOffset+1;
    while(1)
    {
        PCI::BootDiskRead((unsigned int)FileFound, 1, (unsigned int)(CurrentLBA & 0xFFFFFFFF));

        unsigned int NameSize = GetFilePathSize(FileFound);
        struct FileIndex* FilePtr = (struct FileIndex*)((unsigned int)FileFound+NameSize+1);
        if(FilePtr->MetaData[0] & 1)
        {
            RetNumbers[NumberFilesFree] = CurrentLBA;
            NumberFilesFree++;
        }

        CurrentLBA = FilePtr->NextLBALow | ((unsigned long long)FilePtr->NextLBAHigh << 32);
        if(CurrentLBA == 0)
        {
            break;
        }
        CurrentLBA += FileSystemHeader->LbaOffset+1;
    }

    *NumberFound = NumberFilesFree;
    return RetNumbers;

}

unsigned long long FS::GetNextConnecterLBA()
{
    char* FileFound = (char*) Kmalloc::Malloc(512);
    if(FileFound == 0)
    {
        Console::PrintString("Alloc Error\n");
        return 0;
    }
    

    unsigned long long CurrentLBA = FileSystemHeader->LbaOffset+1;
    while(1)
    {
        unsigned long long OldLBA = CurrentLBA;
        PCI::BootDiskRead((unsigned int)FileFound, 1, (unsigned int)(CurrentLBA & 0xFFFFFFFF));

        unsigned int NameSize = GetFilePathSize(FileFound);
        struct FileIndex* FilePtr = (struct FileIndex*)((unsigned int)FileFound+NameSize+1);

        
        CurrentLBA = FilePtr->NextLBALow | ((unsigned long long)FilePtr->NextLBAHigh << 32);
        if(CurrentLBA == 0)
        {
            return OldLBA;
        }
        CurrentLBA += FileSystemHeader->LbaOffset+1;
    }
    return 0; // broken FS

}

void FS::WriteFile(void* Buffer, unsigned int BufferSize, const char* Path)
{
    unsigned int ExpandedSize = BufferSize;
    while(((double)ExpandedSize/512) != ExpandedSize/512){ExpandedSize++;}

   
    char* TmpBuffer = (char*)Kmalloc::Malloc(ExpandedSize);
    if(TmpBuffer == 0)
    {
        Console::PrintString("Alloc Error\n");
        return;
    }


    for(int i=0; i< BufferSize; i++)
    {
        TmpBuffer[i] = ((char*)Buffer)[i];
    }
    for(int i=0; i< ExpandedSize-BufferSize; i++)
    {
        TmpBuffer[BufferSize+i] = 0;
    }

    unsigned int NumberTablesFound = 0;
    unsigned long long* LBAList = GetFreedIndexTables(&NumberTablesFound);

    char* FreeFileIndexs = (char*) Kmalloc::Malloc(512);
    if(FreeFileIndexs ==0 )
    {
        Console::PrintString("Alloc Error\n");
        return;
    }

    unsigned long long NewFileOldIndex = 0;
    struct FileIndex* FilePtr;
    for(int i=0; i< NumberTablesFound; i++)
    {
        PCI::BootDiskRead((unsigned int)FreeFileIndexs, 1, LBAList[i]);

        unsigned int NameOffset = GetFilePathSize(FreeFileIndexs);
        FilePtr = (struct FileIndex*) ((unsigned int)FreeFileIndexs + NameOffset+1);
        if(FilePtr->NumberSectors > ExpandedSize/512)
        {
            NewFileOldIndex = LBAList[i];
            break;
        }


    }

    if(NewFileOldIndex == 0)
    {
        Console::PrintString("Allocing New Home For File\n");

        unsigned long long StartLBA = (FileSystemHeader->NextFreeLBA + FileSystemHeader->LbaOffset)+1;
        unsigned int FileSizeSectors = (ExpandedSize/512);
        FileSystemHeader->NextFreeLBA += FileSizeSectors + 1; // move the fs back 

        // first we write the file, then the index

        unsigned long long FileLBA = StartLBA;

        PCI::BootDiskWrite((unsigned int)TmpBuffer, FileSizeSectors, FileLBA);


        unsigned long long IndexLBA = (StartLBA+(unsigned long long)FileSizeSectors);

        unsigned int PathSize = GetFilePathSize((void*)Path);
        char TmpFileIndex[512] = {0};
        for(int i=1; i< PathSize; i++)
        {
            TmpFileIndex[i-1] = Path[i];
        }
        TmpFileIndex[PathSize-1] = 0;



        struct FileIndex FileIndexHeader = {0};
        FileIndexHeader.LBALow = (FileLBA) & 0xFFFFFFFF;
        FileIndexHeader.LBAHigh = (((FileLBA) >> 32) & 0xFFFF);
        FileIndexHeader.NumberSectors = FileSizeSectors;

        *(unsigned int*)((unsigned int)&FileIndexHeader.MetaData+1) = Checksum(TmpBuffer, ExpandedSize);
        FileIndexHeader.NextLBAHigh = 0;
        FileIndexHeader.NextLBALow = 0;

        for(int i=0; i< sizeof(struct FileIndex); i++)
        {
            TmpFileIndex[PathSize+i] = ((char*)&FileIndexHeader)[i];
        }

        PCI::BootDiskWrite((unsigned int)TmpFileIndex, 1, IndexLBA);


        unsigned long long Connector = GetNextConnecterLBA();
        if(Connector == 0)
        {
            Console::PrintString("broken FS Error\n");
            while(1);
        }

        unsigned long long RelitiveOffset = (IndexLBA-FileSystemHeader->LbaOffset)-1;


        PCI::BootDiskRead((unsigned int)TmpFileIndex, 1, Connector);
        PathSize = GetFilePathSize(TmpFileIndex);
        FilePtr = (struct FileIndex*)((unsigned int)TmpFileIndex+PathSize+1);
        FilePtr->NextLBALow = ((RelitiveOffset) & 0xFFFFFFFF);
        FilePtr->NextLBAHigh = 0;

        PCI::BootDiskWrite((unsigned int)TmpFileIndex, 1, Connector);

        Console::PrintString("Created File Index and linked connector to it!\n");


        Console::PrintString("Updating Filesystem Globally...");

        PCI::BootDiskWrite((unsigned int)FileSystemHeader, 1, FileSystemHeader->LbaOffset);

        Console::PrintString("Done\n");

        Kmalloc::Free(TmpBuffer);
        Kmalloc::Free(FreeFileIndexs);
        Kmalloc::Free(LBAList);

        unsigned long long TmpLBA = FindFileLBA("/NewFile.txt");

        Console::PrintString("Got File\n");

        return;

    }

    unsigned long long FreedFileLBA = FilePtr->LBALow | ((unsigned long long)FilePtr->LBAHigh << 32);
    unsigned int FreedFileSize = FilePtr->NumberSectors;

    unsigned long long NewFileLBA = FreedFileLBA + FileSystemHeader->LbaOffset;
    unsigned long long NewFileOffset = FreedFileLBA;


    FreedFileLBA+=(ExpandedSize/512);
    FreedFileSize-=(ExpandedSize/512);

    FilePtr->LBALow = ((unsigned int)(FreedFileLBA & 0xFFFFFFFF));
    FilePtr->LBAHigh = (unsigned short)((FreedFileLBA >> 32) & 0xFFFF);
    FilePtr->NumberSectors = FreedFileSize;

    PCI::BootDiskWrite((unsigned int)FreeFileIndexs, 1, NewFileOldIndex); // update the index to prevent overwrites





    FileSystemHeader->NextFreeLBA++; // alloc new room for the new header!

    unsigned int FileCheckSum = Checksum(TmpBuffer, ExpandedSize);
    
    struct FileIndex NewFileIndex= {0};
    NewFileIndex.LBALow = NewFileOffset & 0xFFFFFFFF;
    NewFileIndex.LBAHigh = (NewFileOffset >> 32) & 0xFFFF;
    *(unsigned int*)((unsigned int)&NewFileIndex.MetaData+1) = FileCheckSum;
    NewFileIndex.NumberSectors = (ExpandedSize/512);
    NewFileIndex.NextLBAHigh =0;
    NewFileIndex.NextLBALow = 0;

    char* FinnalFileIndex = (char*) Kmalloc::Malloc(512);
    unsigned int PathSize = 0;
    while(Path[PathSize]!=0)
    {
        FinnalFileIndex[PathSize] = Path[PathSize+1];
        PathSize++;
    }

    for(int i=0; i< sizeof(struct FileIndex); i++)
    {
        FinnalFileIndex[PathSize+i] = ((char*)&NewFileIndex)[i]; 
    }

    PCI::BootDiskWrite((unsigned int)FinnalFileIndex, 1, (FileSystemHeader->NextFreeLBA+FileSystemHeader->LbaOffset-1));
    
    Console::PrintString("Created New Header At LBA:");
    Console::PrintInt(FileSystemHeader->LbaOffset+FileSystemHeader->NextFreeLBA-1, 0);
    Console::PrintString("\n");

    unsigned long long ConnecterLBA = GetNextConnecterLBA();
    if(ConnecterLBA == 0)
    {
        Console::PrintString("Broken FS Error\n");
        while(1);
    }
    

    Console::PrintString("Updating Connencter LBA At:");
    Console::PrintInt(ConnecterLBA, 0);
    Console::PrintString("\n");

    PCI::BootDiskRead((unsigned int)FinnalFileIndex, 1, ConnecterLBA);
    PathSize = GetFilePathSize(FinnalFileIndex);
    FilePtr = (struct FileIndex*)((unsigned int)FinnalFileIndex+PathSize+1);
    FilePtr->NextLBALow = (FileSystemHeader->NextFreeLBA-2 & 0xFFFFFFFF);
    FilePtr->NextLBAHigh = (((FileSystemHeader->NextFreeLBA-2) << 32) & 0xFFFF);

    PCI::BootDiskWrite((unsigned int)FinnalFileIndex, 1, ConnecterLBA);

    PCI::BootDiskWrite((unsigned int) TmpBuffer, ExpandedSize/512, NewFileLBA);


    Console::PrintString("File Created Updating Global Next State...");

    PCI::BootDiskWrite((unsigned int)FileSystemHeader, 1, FileSystemHeader->LbaOffset);

    Console::PrintString("Done\n");


    Console::PrintString("Attempting To Read Newly Created File At Path:");
    Console::PrintString(Path);
    Console::PrintString("\n");
    
    unsigned long long TestLBA = FindFileLBA(Path);

    if(TestLBA == 0)
    {
        Console::PrintString("Error File Broken Cant Find\n");
        while(1);
    }




    Kmalloc::Free(TmpBuffer);
    Kmalloc::Free(FinnalFileIndex);
    Kmalloc::Free(FreeFileIndexs);
    Kmalloc::Free(LBAList);
}


void* FS::ReadFileFromLBA(unsigned long long LBA)
{
    
    char FileIndexHeader[512] = {0};

    PCI::BootDiskRead((unsigned int)FileIndexHeader, 1, LBA);

    unsigned int PathSize = GetFilePathSize(FileIndexHeader);

    struct FileIndex* FileIndexPtr = (struct FileIndex*)((unsigned int)FileIndexHeader+PathSize+1);

    Console::PrintString("Finnal File Size:");
    Console::PrintInt(FileIndexPtr->NumberSectors*512, 0);
    Console::PrintString("\n");
    void* FinnalFileOutput = Kmalloc::Malloc(FileIndexPtr->NumberSectors*512);    
    unsigned long long FileLBA = (FileIndexPtr->LBALow | ((unsigned long long)FileIndexPtr->LBAHigh << 32)) + FileSystemHeader->LbaOffset;

    PCI::BootDiskRead((unsigned int)FinnalFileOutput, FileIndexPtr->NumberSectors, (unsigned int)(FileLBA & 0xFFFFFFFF));

    unsigned int CheckSum = *(unsigned int*)((unsigned int)FileIndexPtr->MetaData+1);
    unsigned int CheckSumGot =  FS::Checksum(FinnalFileOutput, FileIndexPtr->NumberSectors*512);

    if(CheckSum != CheckSumGot)
    {
        Console::PrintString("Checksum Failed\n");
        Console::PrintString("Expected CheckSum:");
        Console::PrintInt(CheckSum, 0);
        Console::PrintString("\nGot:");
        Console::PrintInt(CheckSumGot, 0);
        Console::PrintString("\n");
        Console::PrintString((char*)FinnalFileOutput);
        Console::PrintString("\n");
        Kmalloc::Free(FinnalFileOutput);
        return 0;
    }


    Kmalloc::Free(FileIndexHeader);
    return FinnalFileOutput;
}


// THIS WILL GET YOU THE FILE INDEX, NOT THE FILE
unsigned long long FS::FindFileLBA(const char* Path)
{

    //Console::PrintString(Path);
    //Console::PrintString("\n");

    unsigned int NumberFilesFound = 0;
    unsigned long long* LBAsFound = ScanDir(Path, &NumberFilesFound);
    if(NumberFilesFound == 0 || LBAsFound == 0)
    {
        Console::PrintString("NO Such File Or Folder\n");
        return 0; // NONE FOUND
    }

    Console::PrintString("Found ");
    Console::PrintInt(NumberFilesFound, 0);
    Console::PrintString(" Number Files With Close Name\n");

    for(int i=0; i< NumberFilesFound; i++)
    {
        Console::PrintInt(LBAsFound[i], 0);
        Console::PrintString(",");
    }
    Console::PrintString("\n");

    unsigned int PathSize = 0;
    while(Path[PathSize] != 0){PathSize++;}

    char FileIndexFound[512] = {0};
    //Console::PrintString("Current FS Offset:");
    //Console::PrintInt(FileSystemHeader->LbaOffset, 0);
    //Console::PrintString("\n");

    unsigned int VaildLBA = 0;
    for(int i=0; i< NumberFilesFound; i++)
    {
        PCI::BootDiskRead(((unsigned int)FileIndexFound), 1, (unsigned int)(LBAsFound[i] & 0xffffffff));

        unsigned int CurrentFilePathSize = 0;
        while(FileIndexFound[CurrentFilePathSize] != 0){CurrentFilePathSize++;}

        //Console::PrintString("Checking LBA:");
        //Console::PrintInt(LBAsFound[i], 0);
        //Console::PrintString("\n");

        if(CurrentFilePathSize != (PathSize-1))
        {
            continue; // not the same
        }

        for(int j=1; j< (PathSize-1); j++)
        {
            if(FileIndexFound[j-1] != Path[j])
            {
                continue; // name not same
            }
        }

        struct FileIndex* FileIndexHeader = (struct FileIndex*)((unsigned int)FileIndexFound+CurrentFilePathSize+1);
        if(FileIndexHeader->MetaData[0] & 1)
        {
            break; // not a file
        }
        VaildLBA = LBAsFound[i];
        //Console::PrintString("Found Vaild LBA:");
        //Console::PrintInt(VaildLBA, 0);
        //Console::PrintString("\n");

    }
    if(VaildLBA == 0)
    {
        Console::PrintString("Not A Vaild File\n");
    }
    Kmalloc::Free(LBAsFound);
    Kmalloc::Free(FileIndexFound);
    Console::PrintString("Found File Index At LBA:");
    Console::PrintInt(VaildLBA, 0);
    Console::PrintString("\n");
    
    return VaildLBA;

}


unsigned int FS::GetFilePathSize(void* File)
{
    char* FileChar = (char*) File;
    unsigned int Index = 0;
    while(FileChar[Index] != 0){Index++;}
    return Index;
}

unsigned int FS::Checksum(void* FS, unsigned int SizeBytes)
{
    unsigned int Sum = 0;
    for(int i=0; i< SizeBytes; i++)
    {
        unsigned int OldSum = Sum;
        unsigned int Byte = (((char*)FS)[i] & 0xff);
        Sum += Byte;
    }
    return Sum;
}


void FS::RemoveFile(unsigned long long FileIndexLBA)
{
    char FileIndex[512];
    if(FileIndex == 0)
    {
        Console::PrintString("Cant Alloc Memory\n");
        return;
    }
    //Console::PrintString("Reading Disk\n");
    PCI::BootDiskRead((unsigned int)FileIndex, 1, FileIndexLBA);

    unsigned int NameSize = 0;
    while(FileIndex[NameSize] != 0){NameSize++;}

    struct FileIndex* IndexTable = (struct FileIndex*) ((unsigned int)FileIndex + NameSize+1);
    
    IndexTable->MetaData[0] = 1; // mark as freed

    for(int i=0; i< NameSize; i++)
    {
        FileIndex[i] = 0xff; // clear out the name buffer
    }
    FileIndex[NameSize] = 0;
    //Console::PrintString("Writing Disk\n");
    PCI::BootDiskWrite((unsigned int)FileIndex, 1, FileIndexLBA);

    //Console::PrintString("Removed File\n");

}