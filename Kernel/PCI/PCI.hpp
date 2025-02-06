struct PcieMmioData
{
    unsigned long long Address;
    unsigned short PciSegmentGroup;
    unsigned char StartPciNumber;
    unsigned char EndPciNumber;
    unsigned int unused;
} __attribute((packed));


struct PciDevice
{
    unsigned int DeviceIndex; // os given
    unsigned int DeviceID; // vendorID | (deviceID << 16)
    unsigned int DeviceLocasion; // bus | (device << 8) | (func << 16) 
    unsigned int ClassCode; // class | subclass << 8 | ProgIF << 16
};
typedef unsigned char (*DriverFunc)();
typedef void (*DiskReadDriverType)(unsigned int Buffer, unsigned int NumberSectors, unsigned int LBA);

class PCI
{
    public:
    static DiskReadDriverType BootDiskRead;
    static DiskReadDriverType BootDiskWrite;

    static struct PciDevice* PciDevs;
    static unsigned int NumberPCIDevices;
    static void FindDevices();
    static unsigned int ReadPCI(unsigned char bus, unsigned char device, unsigned char func, unsigned char offset);
    static void WritePCI(unsigned char bus, unsigned char device, unsigned char func, unsigned char offset, unsigned int data);
    static void ScanPCI();
    static void FindKnownDrivers();
    
};


#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC




//                       Class      SubClass    ProfIf(0xff for doesnt matter)
#define ATADriverClass ((0x1) | (0x5 << 8) | (0xff << 16))
#define IDRDriverClass ((0x1) | (0x1 << 8) | (0xff << 16))