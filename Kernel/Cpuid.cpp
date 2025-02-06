
extern "C" void Cpuid(unsigned int Index, unsigned int *a, unsigned int* b, unsigned int* c, unsigned int* d)
{
    asm volatile("cpuid" : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d): "a"(Index) );
}

extern "C" void GetMSR(unsigned int Index, unsigned int* Lo, unsigned int* Hi)
{
    asm volatile("rdmsr" : "=a"(*Lo), "=d"(*Hi) : "c"(Index));
}

extern "C" void SetMSR(unsigned int Index, unsigned int Lo, unsigned int Hi)
{
    asm volatile("wrmsr" :: "a"(Lo), "d"(Hi), "c"(Index));
}
