#include "tss.h"

static tss_entry tss = {0};

tss_entry* setup_tss()
{
    tss.esp0 = 0x0;
    tss.ss0 = 0x10;
    tss.esp1 = 0x0;
    tss.ss1 = 0x10;
    tss.esp2 = 0x0;
    tss.ss2 = 0x10;
    return &tss;
}
