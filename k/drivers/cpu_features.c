#include "cpu_features.h"
#include <cpuid.h>

void get_cpuid(enum CPUID_Request request, u32 *eax, u32 *ebx, u32 *ecx,
	       u32 *edx)
{
	__get_cpuid((int)request, eax, ebx, ecx, edx);
}

int check_cpuid_edx(enum CPUID_Feature_EDX feature)
{
	unsigned int eax, ebx, ecx, edx = 0;
	get_cpuid(CPUID_Request_Features, &eax, &ebx, &ecx, &edx);
	return edx & feature;
}
int check_cpuid_ecx(enum CPUID_Feature_ECX feature)
{
	unsigned int eax, ebx, ecx, edx = 0;
	get_cpuid(CPUID_Request_Features, &eax, &ebx, &ecx, &edx);
	return ecx & feature;
}
