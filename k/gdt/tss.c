#include "tss.h"

static tss_entry tss;

tss_entry* setup_tss()
{
    return &tss;
}
