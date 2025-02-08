typedef void (*Ps2ReadDoneFuncType)();


class Ps2
{
    public:
    static void Init();
    static void OnPs2CheckCycle();
    static void SetPs2InputBuffer(void* Buffer, unsigned int BufferSize, void* ThreadObj);
    static void SetPs2InputBuffer(void* Buffer, unsigned int BufferSize, unsigned char* IsDone);
    static Ps2ReadDoneFuncType Ps2ReadDone;
    static unsigned char Ps2KeyPressed;
    static void SetPs2BufferSyscall(void* Buffer, unsigned int BufferSize);
};

