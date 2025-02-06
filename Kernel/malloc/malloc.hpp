

struct MallocOverWrite
{
    unsigned int MemoryAddress;
    unsigned int NumberBytes;
};


class Kmalloc
{
    public:
    struct MallocTable
    {
        void* MemoryAddr;
        unsigned int InUse;
        unsigned int StartingSize;
        unsigned int SizeBytes;
        unsigned int Flags;
        // bit0: IsFree
        // bit1: IsLower
        // bit2: Cant Be Freed ( normally due to defraging)
        // bit3: Remove once possible
    };
    static struct MallocOverWrite* MallocOverWrites;
    static void MallocInit();
    static void* Malloc(unsigned int);
    static void Free(void*);
    static void AddOverwrite(struct MallocOverWrite Table);
    static void CheckIfOverlap();

    private:
    static struct MallocTable* GetTableOfAddr(void*);
    static struct MallocTable* GetNextFreeTable();
    static void ClearMallocTable(struct MallocTable*);
    static struct MallocTable* GetTableOfSize(unsigned int);
    static struct MallocTable* GetTableCloseist(struct MallocTable*, unsigned int);
    
};

