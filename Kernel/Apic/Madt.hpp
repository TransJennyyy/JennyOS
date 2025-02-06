


class Madt
{
    
    
    static void AddToDeviceList(unsigned char EntryType, void* Header);
    static void DefineLAPIC(void* HeaderStart);
    static void DefineDevice(unsigned char EntryType, void* Header);
    static void DisablePIC();
    public:
    static unsigned int AcpiVersionNumber;
    static void* LAPICREGAddr;
    static void ScanMadt(void* MadtStart);
    static void** Devices;
    static unsigned char* DeviceTypes;
    static unsigned int NumberDevices;
};

