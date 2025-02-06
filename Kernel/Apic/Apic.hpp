

struct LAPIC
{
   unsigned char AcpiID;
   unsigned char ApicID;
   unsigned int Flags;
} __attribute__((packed)) ;

struct IOAPIC
{
    unsigned char IoApicID;
    unsigned char Unused;
    unsigned int ApicAddr;
    unsigned int GlobalSystemBase;
} __attribute__((packed));

struct IOAPICINTOVER
{
    unsigned char Bus;
    unsigned char IRQ;
    unsigned int GlobalSystemInt;
    unsigned short Flags;
} __attribute__((packed));

struct IOAPICNMI
{
    unsigned char Source;
    unsigned char Unused;
    unsigned short Flags;
    unsigned int GlobalSystemInt;
} __attribute__((packed));

struct LAPICNMI
{
    unsigned char AcpiID;
    unsigned short Flags;
    unsigned char LINT;
} __attribute__((packed));

struct RSDP_t {
        char Signature[8];
        unsigned char Checksum;
        char OEMID[6];
        unsigned char Revision;
        unsigned int RsdtAddress;
    } __attribute__ ((packed));

struct ACPISDTHeader {
  char Signature[4];
  unsigned int Length;
  unsigned char Revision;
  unsigned char Checksum;
  char OEMID[6];
  char OEMTableID[8];
  unsigned int OEMRevision;
  unsigned int CreatorID;
  unsigned int CreatorRevision;
} __attribute__((packed)) ;

struct XSDP_t {
 char Signature[8];
 unsigned char Checksum;
 char OEMID[6];
 unsigned char Revision;
 unsigned int RsdtAddress;      // deprecated since version 2.0

 unsigned int Length;
 unsigned long long XsdtAddress;
 unsigned char ExtendedChecksum;
 unsigned char reserved[3];
} __attribute__ ((packed));


struct HpetAddressStructure
{
        unsigned char address_space_id;    // 0 - system memory, 1 - system I/O
        unsigned char register_bit_width;
        unsigned char register_bit_offset;
        unsigned char reserved;
        unsigned long long address;
} __attribute__((packed));


struct HpetTable{
        unsigned char hardware_rev_id;
        unsigned char comparator_count:5;
        unsigned char counter_size:1;
        unsigned char reserved:1;
        unsigned char legacy_replacement:1;
        unsigned short pci_vendor_id;
        struct HpetAddressStructure address;
        unsigned char hpet_number;
        unsigned short minimum_tick;
        unsigned char page_protection;
} __attribute__((packed));


class Apic
{
#define NumberDevicesNeeded 1
    static const char* NeededDeviceSigs[NumberDevicesNeeded]; 
    
    
    static void FindRDSP();
    static void ScanRSDT();
    public:
    static unsigned int NumberCyclesPerMs;
    static unsigned int NumberRSDTEntrys;
    static unsigned int* EntryPtrs;
    static void* RdspPtr;
    static void WriteLAPICReg(unsigned int Offset, unsigned int Data);
    static unsigned int ReadLAPICReg(unsigned int Offset);
    static void WriteIOAPIC(void* IoApicBase, unsigned int data, unsigned int offset);
    static unsigned int ReadIOAPIC(void* IoApicBase, unsigned int offset);
    static unsigned char HasMSR;
    static void init();
    static void* FindRSDTEntryWithSig(char[4]);
    static unsigned char CheckSum(void*, unsigned int);
    static void StartLAPICS();    
    static void StartIOAPICS();
    static void UnmaskIRQ(unsigned int GlobalSystemInt);
    static void MaskIRQ(unsigned int GlobalSystemInt);
    static unsigned int ConvertIRQToGSI(unsigned char IRQ, unsigned char* WasFound, unsigned int* DeviceIndex);
    static void StartLApicTimer(unsigned int NumberTimesPerMS);
};


