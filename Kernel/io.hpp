
class IO
{
    public:
    static void iowait();
    static void outb(unsigned short, unsigned char);
    static void outs(unsigned short, unsigned short);
    static void outl(unsigned short, unsigned int);

    static unsigned char inb(unsigned short);
    static unsigned short ins(unsigned short);
    static unsigned int inl(unsigned short);
};

