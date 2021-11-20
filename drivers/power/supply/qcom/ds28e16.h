#ifndef _DS28E16_H_
#define _DS28E16_H_

#include <soc/qcom/socinfo_xiaomi.h>

#ifdef CONFIG_MACH_XIAOMI_SM8250
#define USES_DS28E16 \
	(socinfo_get_platform_type() == HW_PLATFORM_ALIOTH || \
	 socinfo_get_platform_type() == HW_PLATFORM_APOLLO || \
	 socinfo_get_platform_type() == HW_PLATFORM_LMI || \
	 socinfo_get_platform_type() == HW_PLATFORM_THYME || \
	 socinfo_get_platform_type() == HW_PLATFORM_UMI)
#else
#define USES_DS28E16 (0)
#endif

#endif
