
extern "C" void Cpuid(unsigned int Index, unsigned int* a, unsigned int* b, unsigned int* c, unsigned int* d);

extern "C" void SetMSR(unsigned int Index, unsigned int Lo, unsigned int Hi);

extern "C" void GetMSR(unsigned int Index, unsigned int* Lo, unsigned int* Hi);
