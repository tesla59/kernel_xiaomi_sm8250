/*
 * wm_adsp.h  --  Wolfson ADSP support
 *
 * Copyright 2012 Wolfson Microelectronics plc
 *
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __WM_ADSP_H
#define __WM_ADSP_H

#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/compress_driver.h>

#include "wmfw.h"

/* Return values for wm_adsp_compr_handle_irq_v2 */
#define WM_ADSP_COMPR_OK                 0
#define WM_ADSP_COMPR_VOICE_TRIGGER      1

#define WM_ADSP2_REGION_0 BIT(0)
#define WM_ADSP2_REGION_1 BIT(1)
#define WM_ADSP2_REGION_2 BIT(2)
#define WM_ADSP2_REGION_3 BIT(3)
#define WM_ADSP2_REGION_4 BIT(4)
#define WM_ADSP2_REGION_5 BIT(5)
#define WM_ADSP2_REGION_6 BIT(6)
#define WM_ADSP2_REGION_7 BIT(7)
#define WM_ADSP2_REGION_8 BIT(8)
#define WM_ADSP2_REGION_9 BIT(9)
#define WM_ADSP2_REGION_1_9 (WM_ADSP2_REGION_1 | \
		WM_ADSP2_REGION_2 | WM_ADSP2_REGION_3 | \
		WM_ADSP2_REGION_4 | WM_ADSP2_REGION_5 | \
		WM_ADSP2_REGION_6 | WM_ADSP2_REGION_7 | \
		WM_ADSP2_REGION_8 | WM_ADSP2_REGION_9)
#define WM_ADSP2_REGION_ALL (WM_ADSP2_REGION_0 | WM_ADSP2_REGION_1_9)

struct wm_adsp_region_v2 {
	int type;
	unsigned int base;
};

struct wm_adsp_alg_region_v2 {
	struct list_head list;
	unsigned int alg;
	int type;
	unsigned int base;
};

struct wm_adsp_compr_v2;
struct wm_adsp_compr_buf_v2;
struct wm_adsp_ops_v2;

struct wm_adsp_v2 {
	const char *part;
	const char *name;
	const char *fwf_name;
	int rev;
	int num;
	int type;
	struct device *dev;
	struct regmap *regmap;
	struct snd_soc_component *component;

	struct wm_adsp_ops_v2 *ops;

	unsigned int base;
	unsigned int base_sysinfo;
	unsigned int sysclk_reg;
	unsigned int sysclk_mask;
	unsigned int sysclk_shift;

	// Speaker calibration parameters
	int cal_ambient;
	int cal_r;
	int cal_status;
	int cal_checksum;

	struct list_head alg_regions;

	unsigned int fw_id;
	unsigned int fw_id_version;
	unsigned int fw_vendor_id;

	const struct wm_adsp_region_v2 *mem;
	int num_mems;

	int fw;
	int fw_ver;

	bool no_preloader;
	bool preloaded;
	bool booted;
	bool running;
	bool fatal_error;
	bool tuning_has_prefix;

	struct list_head ctl_list;

	struct work_struct boot_work;

	struct list_head compr_list;
	struct list_head buffer_list;

	struct mutex pwr_lock;

	unsigned int lock_regions;

	unsigned int n_rx_channels;
	unsigned int n_tx_channels;

	struct mutex *rate_lock;
	u8 *rx_rate_cache;
	u8 *tx_rate_cache;

#ifdef CONFIG_DEBUG_FS
	struct dentry *debugfs_root;
	char *wmfw_file_name;
	char *bin_file_name;
#endif
	unsigned int data_word_mask;
	int data_word_size;
};

struct wm_adsp_ops_v2 {
	unsigned int sys_config_size;

	bool (*validate_version)(struct wm_adsp_v2 *dsp, unsigned int version);
	unsigned int (*parse_sizes)(struct wm_adsp_v2 *dsp,
				    const char * const file,
				    unsigned int pos,
				    const struct firmware *firmware);
	int (*setup_algs)(struct wm_adsp_v2 *dsp);
	unsigned int (*region_to_reg)(struct wm_adsp_region_v2 const *mem,
				      unsigned int offset);

	void (*show_fw_status)(struct wm_adsp_v2 *dsp);
	void (*stop_watchdog)(struct wm_adsp_v2 *dsp);

	int (*enable_memory)(struct wm_adsp_v2 *dsp);
	void (*disable_memory)(struct wm_adsp_v2 *dsp);
	int (*lock_memory)(struct wm_adsp_v2 *dsp, unsigned int lock_regions);

	int (*enable_core)(struct wm_adsp_v2 *dsp);
	void (*disable_core)(struct wm_adsp_v2 *dsp);

	int (*start_core)(struct wm_adsp_v2 *dsp);
	void (*stop_core)(struct wm_adsp_v2 *dsp);
};

#define WM_ADSP_PRELOADER(wname, num, event_fn) \
{	.id = snd_soc_dapm_supply, .name = wname " Preloader", \
	.reg = SND_SOC_NOPM, .shift = num, .event = event_fn, \
	.event_flags = SND_SOC_DAPM_PRE_PMU | SND_SOC_DAPM_PRE_PMD, \
	.subseq = 100, /* Ensure we run after SYSCLK supply widget */ }

#define WM_ADSP1(wname, num) \
	SND_SOC_DAPM_PGA_E(wname, SND_SOC_NOPM, num, 0, NULL, 0, \
		wm_adsp1_event_v2, SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD)

#define WM_ADSP2_PRELOAD_SWITCH(wname, num) \
	SOC_SINGLE_EXT(wname " Preload Switch", SND_SOC_NOPM, num, 1, 0, \
		wm_adsp2_preloader_get_v2, wm_adsp2_preloader_put_v2)

#define WM_ADSP2(wname, num, event_fn) \
	SND_SOC_DAPM_SPK(wname " Preload", NULL), \
	WM_ADSP_PRELOADER(wname, num, event_fn), \
{	.id = snd_soc_dapm_out_drv, .name = wname, \
	.reg = SND_SOC_NOPM, .shift = num, .event = wm_adsp_event_v2, \
	.event_flags = SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMD }

#define WM_HALO(wname, num, event_fn) \
	WM_ADSP2(wname, num, event_fn)

#define WM_ADSP_FW_CONTROL(dspname, num) \
	SOC_ENUM_EXT(dspname " Firmware", wm_adsp_fw_enum[num], \
		     wm_adsp_fw_get_v2, wm_adsp_fw_put_v2)

extern const struct soc_enum wm_adsp_fw_enum[];

int wm_adsp1_init_v2(struct wm_adsp_v2 *dsp);
int wm_adsp2_init_v2(struct wm_adsp_v2 *dsp);
void wm_adsp2_remove_v2(struct wm_adsp_v2 *dsp);
int wm_adsp2_component_probe_v2(struct wm_adsp_v2 *dsp, struct snd_soc_component *component);
int wm_adsp2_component_remove_v2(struct wm_adsp_v2 *dsp, struct snd_soc_component *component);
void wm_adsp_queue_boot_work_v2(struct wm_adsp_v2 *dsp);
int wm_vpu_init_v2(struct wm_adsp_v2 *vpu);
int wm_halo_init_v2(struct wm_adsp_v2 *dsp, struct mutex *rate_lock);

int wm_adsp1_event_v2(struct snd_soc_dapm_widget *w,
		   struct snd_kcontrol *kcontrol, int event);

int wm_adsp_early_event_v2(struct snd_soc_dapm_widget *w,
			struct snd_kcontrol *kcontrol, int event);

irqreturn_t wm_adsp2_bus_error_v2(struct wm_adsp_v2 *adsp);
irqreturn_t wm_halo_bus_error_v2(struct wm_adsp_v2 *dsp);
irqreturn_t wm_halo_wdt_expire_v2(int irq, void *data);

int wm_adsp_event_v2(struct snd_soc_dapm_widget *w,
		  struct snd_kcontrol *kcontrol, int event);
int wm_vpu_event_v2(struct snd_soc_dapm_widget *w,
		 struct snd_kcontrol *kcontrol, int event);

int wm_adsp2_set_dspclk_v2(struct snd_soc_dapm_widget *w, unsigned int freq);

int wm_adsp2_preloader_get_v2(struct snd_kcontrol *kcontrol,
			   struct snd_ctl_elem_value *ucontrol);
int wm_adsp2_preloader_put_v2(struct snd_kcontrol *kcontrol,
			   struct snd_ctl_elem_value *ucontrol);
int wm_adsp_fw_get_v2(struct snd_kcontrol *kcontrol,
		   struct snd_ctl_elem_value *ucontrol);
int wm_adsp_fw_put_v2(struct snd_kcontrol *kcontrol,
		   struct snd_ctl_elem_value *ucontrol);

int wm_adsp_compr_open_v2(struct wm_adsp_v2 *dsp, struct snd_compr_stream *stream);
int wm_adsp_compr_free_v2(struct snd_compr_stream *stream);
int wm_adsp_compr_set_params_v2(struct snd_compr_stream *stream,
			     struct snd_compr_params *params);
int wm_adsp_compr_get_caps_v2(struct snd_compr_stream *stream,
			   struct snd_compr_caps *caps);
int wm_adsp_compr_trigger_v2(struct snd_compr_stream *stream, int cmd);
int wm_adsp_compr_handle_irq_v2(struct wm_adsp_v2 *dsp);
int wm_adsp_compr_pointer_v2(struct snd_compr_stream *stream,
			  struct snd_compr_tstamp *tstamp);
int wm_adsp_compr_copy_v2(struct snd_compr_stream *stream,
		       char __user *buf, size_t count);
int wm_adsp_write_ctl_v2(struct wm_adsp_v2 *dsp, const char *name, const void *buf,
		      size_t len);
int wm_adsp_read_ctl_v2(struct wm_adsp_v2 *dsp, const char *name, void *buf,
		     size_t len);

#endif
