
class RamDiskMgr
{
    #define NumberOverides 5
    public:
    static char* OverrideArray;
    static void* OverrideAddr[NumberOverides];
    static void init();
    static void* ReadFile(const char* Path); // will either passthogh a ramdisk app or a real file from the fs

    static void PrintSyscall(void* Base, const char* String)
    {
        unsigned int memoryAddress = (unsigned int)String - (unsigned int)Base;

        asm("int $0x80" :: "S"(0), "ebx"(memoryAddress));

    }

    static unsigned int LoadAppSyscall(void* Base, const char* String)
    {
        char* RelitivePath = (char*)((unsigned int)String - (unsigned int)Base);
        unsigned int Output;
        asm("int $0x80" : "=S"(Output) : "S"(0x10), "ebx"(RelitivePath));
        return Output;
    }

};