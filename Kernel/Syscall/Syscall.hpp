

typedef void (*SysCallFunc)(unsigned int* RegistorList, void* ThreadOBJ);

class SysCalls
{
    public:
    static SysCallFunc SysCallList[0xFFFF];
    
    static void Init();
};