#include "tss.h"

static tss_entry tss = {0};

tss_entry* setup_tss()
{
    tss.esp0 = 0xFFFFF;
    tss.ss0 = 0x20;
    tss.esp1 = 0xFFFFF;
    tss.ss1 = 0x20;
    tss.esp2 = 0xFFFFF;
    tss.ss2 = 0x20;
    return &tss;
}
