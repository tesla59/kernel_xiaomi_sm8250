// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2009-2020, The Linux Foundation. All rights reserved.
 */

#include <soc/qcom/socinfo_xiaomi.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/soc/qcom/smem.h>
#include <linux/sysfs.h>
#include <linux/types.h>

#define LOG_TAG KBUILD_MODNAME ": "

#define LOGE(msg, ...) pr_err(LOG_TAG msg, ##__VA_ARGS__)
#define LOGI(msg, ...) pr_info(LOG_TAG msg, ##__VA_ARGS__)
#define LOGW(msg, ...) pr_warn(LOG_TAG msg, ##__VA_ARGS__)

#define SMEM_ID_VENDOR1                 135

#define HW_MAJOR_VERSION_SHIFT 16
#define HW_MAJOR_VERSION_MASK  0xFFFF0000
#define HW_MINOR_VERSION_SHIFT 0
#define HW_MINOR_VERSION_MASK  0x0000FFFF
#define HW_COUNTRY_VERSION_MASK 0xFFF00000
#define HW_COUNTRY_VERSION_SHIFT 20
#define HW_BUILD_VERSION_MASK 0x000F0000
#define HW_BUILD_VERSION_SHIFT 16

/*
 * It is recommended to set the lowercase version
 * of what you find in socinfo.c's hw_version
 */
static const char *hw_platform_xiaomi[] = {
#if defined(CONFIG_MACH_XIAOMI_SM8250)
	[HW_PLATFORM_CMI] = "cmi",
	[HW_PLATFORM_LMI] = "lmi",
	[HW_PLATFORM_CAS] = "cas",
	[HW_PLATFORM_APOLLO] = "apollo",
	[HW_PLATFORM_UMI] = "umi",
	[HW_PLATFORM_ALIOTH] = "alioth",
	[HW_PLATFORM_THYME] = "thyme",
	[HW_PLATFORM_ENUMA] = "enuma",
	[HW_PLATFORM_ELISH] = "elish",
#elif defined(CONFIG_MACH_XIAOMI_SM7250)
	[HW_PLATFORM_VANGOGH] = "vangogh",
	[HW_PLATFORM_PICASSO] = "picasso",
	[HW_PLATFORM_MONET]  = "monet",
#endif
};

/*
 * Put here the values that you find in
 * #define HARDWARE_PLATFORM_* in socinfo.h
 *
 * We keep them here to make sure that socinfo_get_platform_type
 * returns what userspace expects
 */
static uint32_t hw_version_platform[] = {
#if defined(CONFIG_MACH_XIAOMI_SM8250)
	[HW_PLATFORM_CMI] = 1,
	[HW_PLATFORM_LMI] = 3,
	[HW_PLATFORM_CAS] = 7,
	[HW_PLATFORM_APOLLO] = 8,
	[HW_PLATFORM_UMI] = 2,
	[HW_PLATFORM_ALIOTH] = 9,
	[HW_PLATFORM_THYME] = 10,
	[HW_PLATFORM_ENUMA] = 11,
	[HW_PLATFORM_ELISH] = 12,
#elif defined(CONFIG_MACH_XIAOMI_SM7250)
	[HW_PLATFORM_VANGOGH] = 2,
	[HW_PLATFORM_PICASSO] = 3,
	[HW_PLATFORM_MONET] = 4,
#endif
};

const char *product_name_get(void)
{
	char *product_name = NULL;
	size_t size;
	uint32_t hw_type;

	hw_type = socinfo_get_platform_type();

	product_name = qcom_smem_get(QCOM_SMEM_HOST_ANY, SMEM_ID_VENDOR1, &size);
	if (IS_ERR_OR_NULL(product_name)) {
		LOGW("Can't find SMEM_ID_VENDOR1; falling back on dummy values.\n");
		return hw_platform_xiaomi[hw_type];
	}

	return product_name;
}
EXPORT_SYMBOL(product_name_get);

uint32_t get_hw_country_version(void)
{
	uint32_t version = socinfo_get_platform_version();
	return (version & HW_COUNTRY_VERSION_MASK) >> HW_COUNTRY_VERSION_SHIFT;
}
EXPORT_SYMBOL(get_hw_country_version);

uint32_t get_hw_version_platform(void)
{
	uint32_t hw_type = socinfo_get_platform_type();
	return hw_version_platform[hw_type];
}
EXPORT_SYMBOL(get_hw_version_platform);

uint32_t get_hw_version_major(void)
{
	uint32_t version = socinfo_get_platform_version();
	return (version & HW_MAJOR_VERSION_MASK) >> HW_MAJOR_VERSION_SHIFT;
}
EXPORT_SYMBOL(get_hw_version_major);

uint32_t get_hw_version_minor(void)
{
	uint32_t version = socinfo_get_platform_version();
	return (version & HW_MINOR_VERSION_MASK) >> HW_MINOR_VERSION_SHIFT;
}
EXPORT_SYMBOL(get_hw_version_minor);

uint32_t get_hw_version_build(void)
{
	uint32_t version = socinfo_get_platform_version();
	return (version & HW_BUILD_VERSION_MASK) >> HW_BUILD_VERSION_SHIFT;
}
EXPORT_SYMBOL(get_hw_version_build);

static ssize_t product_name_show(struct kobject *kobj, 
		struct kobj_attribute *attr, char *buf) {
	return sprintf(buf, "%s", product_name_get());
}

static struct kobj_attribute product_name_attr = __ATTR(product_name, 0444,
		product_name_show, NULL);

static struct kobject *kobj;

static int __init socinfo_xiaomi_init(void) {
	kobj = kobject_create_and_add("socinfo_xiaomi", NULL);

	if (sysfs_create_file(kobj, &product_name_attr.attr))
		LOGE("Cannot create product_name sysfs file\n");

	LOGI("Module loaded, device name: %s\n", product_name_get());

	return 0;
}

static void __exit socinfo_xiaomi_exit(void) {
	sysfs_remove_file(kobj, &product_name_attr.attr);

	kobject_put(kobj);

	return;
}

module_init(socinfo_xiaomi_init);
module_exit(socinfo_xiaomi_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Xiaomi <bsp@xiaomi.com>");
MODULE_AUTHOR("Sebastiano Barezzi <barezzisebastiano@gmail.com>");
MODULE_DESCRIPTION("Xiaomi device info");
