#include "msr.h"

/* Read from an MSR (Model-Specific Register) */
u64 msr_readl(u32 msr)
{
	u32 low, high;
	__asm__ volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
	return ((u64)high << 32) | low;
}

/* Write to an MSR (Model-Specific Register) */
void msr_writel(u32 msr, u64 value)
{
	u32 low = (u32)value;
	u32 high = (u32)(value >> 32);
	__asm__ volatile("wrmsr" ::"c"(msr), "a"(low), "d"(high));
}

void msr_writew(u32 msr, u32 eax, u32 edx)
{
	msr_writel(msr, eax | ((u64)edx << 32));
}
