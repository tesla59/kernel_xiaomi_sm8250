/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2009-2020, The Linux Foundation. All rights reserved.
 */

#ifndef _ARCH_ARM_MACH_MSM_SOCINFO_XIAOMI_H_
#define _ARCH_ARM_MACH_MSM_SOCINFO_XIAOMI_H_

#include <soc/qcom/socinfo.h>

enum {
#if defined(CONFIG_MACH_XIAOMI_SM8250)
	HW_PLATFORM_CMI    = 36,
	HW_PLATFORM_LMI    = 37,
	HW_PLATFORM_CAS    = 41,
	HW_PLATFORM_APOLLO = 42,
	HW_PLATFORM_UMI    = 43,
	HW_PLATFORM_ALIOTH = 44,
	HW_PLATFORM_THYME  = 45,
	HW_PLATFORM_ENUMA  = 46,
	HW_PLATFORM_ELISH  = 47,
#elif defined(CONFIG_MACH_XIAOMI_SM7250)
	HW_PLATFORM_VANGOGH = 35,
	HW_PLATFORM_PICASSO = 36,
	HW_PLATFORM_MONET   = 37,
#endif
};

enum {
	HW_MAJOR_VERSION_B = 9,
	HW_MINOR_VERSION_B = 1,
};

enum {
	CountryCN = 0,
	CountryGlobal = 1,
	CountryIndia = 2,
	INVALID,
};

uint32_t get_hw_version_platform(void);
uint32_t get_hw_country_version(void);
uint32_t get_hw_version_major(void);
uint32_t get_hw_version_minor(void);
uint32_t get_hw_version_build(void);
const char *product_name_get(void);

#endif
