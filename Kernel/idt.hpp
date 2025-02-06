
class IDT
{
    
    public:
    struct IDTEntry
    {
        unsigned short FuncAddrLow;
        unsigned short Segment;
        unsigned char Zero;
        unsigned char TypeData;
        unsigned short FuncAddrHigh;
    }  __attribute__((packed)) ;
    struct IDTR
    {
        unsigned short SizeBytes;
        unsigned int Addr;
    }  __attribute__((packed)) ;
    
    typedef void (*IDTFuncEntry)();
    typedef void (*IDTEntryType)(unsigned int);
    static IDTEntryType CpuIntStormHandler;
    static IDTFuncEntry CpuUnderFlowHandler;
    static char* CoreIDPerCore;
    static void SetISR(unsigned int, IDTEntryType , unsigned char);
    static void InitIDT();
};

