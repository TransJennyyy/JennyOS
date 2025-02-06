struct PageDirectory
{
    /*unsigned char Flags;
    unsigned short AvlMemoryAddressL;
    unsigned char MemoryAddressH;*/
    unsigned int Data;
} __attribute((packed));

struct PageTable
{
    /*unsigned char Flags;
    unsigned short AVLMemoryL;
    unsigned char MemoryH;
    */
   unsigned int Data;
} __attribute((packed));


class Paging
{
    public:
    static unsigned char PagingSetup;
    static struct PageDirectory PageDiretorys[1024];
    static struct PageTable PageTables[1024];
    static unsigned char PageSpinLock;
    static void Init(); // sets up a 1:1 page Table
    static void DisableTmp();
    static void Enable();


};