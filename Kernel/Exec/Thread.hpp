
struct ThreadObject
{
    void* AppStart;
    unsigned int NumberSyscallsPerSecond;
    unsigned int Priority;

    unsigned int CoreUsing; // the Core using will need another core to use if a fail of the main core
    unsigned int BackupCore; // this core will run only when the main core is disabled

    unsigned int StackPTR;
    unsigned int FirstStart;
    unsigned char ThreadPaused; // set to 1 to pause a thread of ThreadWaitTime number LAPIC cycles

    void* ThreadCloseMsg;

    unsigned char BgTask; // just if its closed do we open a command line
    
    unsigned int ThreadWaitTime; // this is the number of LAPIC ints untill it unpauses( to never happen leave as zero)

    unsigned int ThreadState; // 0 for normal

    unsigned int ThreadParentIndex;
};

class Thread
{
    public:
    static unsigned int SpinLock;
    static unsigned int InitThread(void* AppStart, unsigned int Priority, unsigned char IsBGTask);


    void RunThread();
};

