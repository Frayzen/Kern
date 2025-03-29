#include "apic.h"
#include "drivers/cpu_features.h"
#include <cpuid.h>
#include <panic.h>

static int check_apic(void)
{
    unsigned int eax, unused, edx;
    __get_cpuid(1, &eax, &unused, &unused, &edx);
    return edx & CPUID_FEAT_EDX_APIC;
}

void apic_setup(void)
{
  if (!check_apic())
  {
    panic("Cannot handle apic !");
  }
  // TODO disable pic first !
  // TODO find a way to select either apic or pic

  return;
}
