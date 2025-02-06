
class Console
{

    static void NewLine();
    public:
    static unsigned int CurrentOffset;
    static char* VGAbuffer;
    static void VGAInit();
    static void PrintString(const char*);
    static void PrintString(const char*, unsigned char InISR);
    static void PrintInt(unsigned int, unsigned char);    
    static void ScrollScreen();
    static unsigned int VgaSpinLock;
};

