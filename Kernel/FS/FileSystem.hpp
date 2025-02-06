
struct FsHeader
{
    char MagicBits[5];
    unsigned int LbaOffset;
    unsigned long long NextFreeLBA;
} __attribute((packed)) ;


struct FileIndex
{
    unsigned int LBALow;
    unsigned short LBAHigh;
    unsigned int NumberSectors;
    unsigned char MetaData[8];
    unsigned int NextLBALow;
    unsigned short NextLBAHigh;

} __attribute((packed)) ;

class FS
{

    static unsigned char FileSystemSetup;
    static unsigned int FileSystemOffset;
    
    public:
    static struct FsHeader* FileSystemHeader;
    static void init();
    // gives all LBA's within a directory
    static unsigned long long NextFreeLBA;
    static unsigned long long LastIndexTable;
    static unsigned long long* ScanDir(const char* Path, unsigned int* NumberFilesFound);
    static void RemoveFile(unsigned long long FileIndexLBA);
    static unsigned int Checksum(void* FilePTR, unsigned int FileSizeBytes);
    static unsigned int GetFilePathSize(void* FilePTR);
    static unsigned long long FindFileLBA(const char* Path);
    static void* ReadFileFromLBA(unsigned long long LBA);
    static unsigned long long* GetFreedIndexTables(unsigned int* NumberFound);
    static void WriteFile(void* Buffer, unsigned int BufferSize, const char* Path);
    static unsigned long long GetNextConnecterLBA();
    static unsigned int FileSizeFromLBA(unsigned long long LBA);


};