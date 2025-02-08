
struct ThreadObject
{
    void* AppStart;
    unsigned int NumberSyscallsPerSecond;
    unsigned int Priority;

    unsigned int CoreUsing; // the Core using will need another core to use if a fail of the main core
    unsigned int BackupCore; // this core will run only when the main core is disabled

    unsigned int StackPTR;
    unsigned int FirstStart;

    void* ThreadCloseMsg;

    
    unsigned int ThreadWaitTime; // this is the number of LAPIC ints untill it unpauses( to never happen leave as zero)

    unsigned int ThreadState; 
    // 0 for normal
    // 1 for paused
    // 2 for closed
    // 3 for waiting
    unsigned int ThreadParentIndex;
};
#define ThreadStateNormal 0
#define ThreadStatePaused 1
#define ThreadStateClosed 2
#define ThreadStateWaiting 3
#define ThreadStateRestarting 4

class Thread
{
    public:
    static unsigned int SpinLock;
    static unsigned int InitThread(void* AppStart, unsigned int Priority, unsigned char IsBGTask);

    static void SwitchTask(unsigned int CoreIndex, unsigned int ISRStartingStack);
    void RunThread();
};

extern unsigned int NumberThreads;
extern struct ThreadObject* ThreadArray;
