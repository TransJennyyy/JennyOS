#include "Paging.hpp"
#include "../console/console.hpp"
#include "../malloc/malloc.hpp"
#include "../Cpuid.hpp"

struct PageDirectory Paging::PageDiretorys[1024] __attribute__((aligned(4096)));
struct PageTable Paging::PageTables[1024] __attribute__((aligned(4096)));

unsigned char Paging::PagingSetup;
unsigned char Paging::PageSpinLock;


void Paging::DisableTmp()
{
    if(!PagingSetup)
    {
        return;
    }   
    unsigned int CR0;
    PageSpinLock = 1;
    asm volatile("mov %%cr0, %0" : "=a"(CR0));
    CR0 &= ~(0x80000001);
    asm volatile("mov %0, %%cr0" :: "a"(CR0));
}

void Paging::Enable()
{
    if(!PagingSetup)
    {
        return;
    }   
    unsigned int CR3Update = (unsigned int)PageDiretorys;
    asm volatile("mov %0, %%cr3" : : "a"(CR3Update));
    
    unsigned int CR0;
    asm volatile("mov %%cr0, %0" : "=a"(CR0));
    CR0 |= 0x80000001;
    asm volatile("mov %0, %%cr0" :: "a"(CR0));
    PageSpinLock = 0;
}


void Paging::Init()
{
    Console::PrintString("Seting Up 1:1 Page Table's\n");
    PagingSetup = 1;
    PageSpinLock = 0;
    struct PageDirectory* PageDirArray = (struct PageDirectory*) PageDiretorys;
    struct PageTable* PageTableArray = (struct PageTable*) PageTables;

    Console::PrintString("Page Dir Address:");
    Console::PrintInt((unsigned int)PageDiretorys, 1);
    Console::PrintString("\nPage Table Address:");
    Console::PrintInt((unsigned int)PageTables, 1);
    Console::PrintString("\n");




    for(int i=0; i< 1024; i++)
    {
        struct PageDirectory TmpDir = {0};
        TmpDir.Data = (unsigned int)&PageTableArray[i] | 3;
        PageDiretorys[i] = TmpDir;
    }
    unsigned int MemoryAddress = 0;
    for(int i=0; i< 1024; i++)
    {
        struct PageTable TmpTable = {0};
        TmpTable.Data = MemoryAddress | 3;
        PageTableArray[i] = TmpTable;
        MemoryAddress += 4096;
        Console::PrintInt(MemoryAddress, 1);
        Console::PrintString(",");
    }
    unsigned int CR3Update = (unsigned int)PageDiretorys;

    asm volatile("mov %0, %%cr3" : : "a"(CR3Update));
    Console::PrintString("Enabling Paging...");

    unsigned int CR0;
    asm volatile("mov %%cr0, %0" : "=a"(CR0));
    CR0 |= 0x80000001;
    asm volatile("mov %0, %%cr0" :: "a"(CR0));
    
    Console::PrintString("Done\n");    
}