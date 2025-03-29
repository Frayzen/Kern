#ifndef MSR_H
#define MSR_H

#include "k/types.h"

/* Read from an MSR (Model-Specific Register) */
u64 msr_readl(u32 msr);
/* Write to an MSR (Model-Specific Register) */
void msr_writel(u32 msr, u64 value);
void msr_writew(u32 msr, u32 eax, u32 edx);

#endif /* !MSR_H */
