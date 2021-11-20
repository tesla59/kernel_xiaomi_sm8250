#include <drm/drm_print.h>
#include <linux/device.h>
#include <linux/slab.h>

#include "drm_sysfs.h"
#include "drm_internal_mi.h"

#define to_drm_connector(d) dev_get_drvdata(d)

static ssize_t disp_param_store(struct device *device,
			   struct device_attribute *attr,
			   const char *buf, size_t count)
{
	struct drm_connector *connector = to_drm_connector(device);
	char *input_copy, *input_dup = NULL;
	u32 param;
	int ret;

	input_copy = kstrdup(buf, GFP_KERNEL);
	if (!input_copy) {
		DRM_ERROR("can not allocate memory\n");
		ret = -ENOMEM;
		goto exit;
	}
	input_dup = input_copy;
	/* removes leading and trailing whitespace from input_copy */
	input_copy = strim(input_copy);
	ret = kstrtouint(input_copy, 16, &param);
	if (ret) {
		DRM_ERROR("input buffer conversion failed\n");
		ret = -EAGAIN;
		goto exit_free;
	}

	ret = dsi_display_set_disp_param(connector, param);

exit_free:
	kfree(input_dup);
exit:
	return ret ? ret : count;
}

static ssize_t disp_param_show(struct device *device,
			   struct device_attribute *attr,
			   char *buf)
{
	struct drm_connector *connector = to_drm_connector(device);
	u32 param;

	dsi_display_get_disp_param(connector, &param);

	return snprintf(buf, PAGE_SIZE, "0x%08X\n", param);
}

static ssize_t mipi_reg_store(struct device *device,
			   struct device_attribute *attr,
			   const char *buf, size_t count)
{
	struct drm_connector *connector = to_drm_connector(device);
	int ret;

	ret = dsi_display_write_mipi_reg(connector, (char *)buf);

	return ret ? ret : count;
}

static ssize_t mipi_reg_show(struct device *device,
			   struct device_attribute *attr,
			   char *buf)
{
	struct drm_connector *connector = to_drm_connector(device);
	return dsi_display_read_mipi_reg(connector, buf);
}

static ssize_t oled_pmic_id_show(struct device *device,
			   struct device_attribute *attr,
			   char *buf)
{
	struct drm_connector *connector = to_drm_connector(device);
	return dsi_display_read_oled_pmic_id(connector, buf);
}

static ssize_t panel_info_show(struct device *device,
			   struct device_attribute *attr,
			   char *buf)
{
	struct drm_connector *connector = to_drm_connector(device);
	return dsi_display_read_panel_info(connector, buf);
}

static ssize_t wp_info_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct drm_connector *connector = to_drm_connector(dev);
	return dsi_display_read_wp_info(connector, buf);
}

static ssize_t dynamic_fps_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct drm_connector *connector = to_drm_connector(dev);
	return dsi_display_read_dynamic_fps(connector, buf);
}

static ssize_t doze_brightness_store(struct device *device,
			   struct device_attribute *attr,
			   const char *buf, size_t count)
{
	struct drm_connector *connector = to_drm_connector(device);
	int doze_brightness;
	int ret;

	ret = kstrtoint(buf, 0, &doze_brightness);;
	if (ret)
		return ret;

	ret = dsi_display_set_doze_brightness(connector, doze_brightness);

	return ret ? ret : count;
}

static ssize_t doze_brightness_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct drm_connector *connector = to_drm_connector(dev);
	return dsi_display_get_doze_brightness(connector, buf);
}

static ssize_t gamma_test_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct drm_connector *connector = to_drm_connector(dev);
	int ret = 0;

	ret = dsi_display_read_gamma_param(connector);
	if (ret) {
		pr_err("Failed to update panel id and gamma para!\n");
	}

	ret = dsi_display_print_gamma_param(connector, buf);
	return ret;
}

extern ssize_t smart_fps_value_show(struct device *device,
			   struct device_attribute *attr,
			   char *buf);

static ssize_t fod_ui_ready_show(struct device *device,
			   struct device_attribute *attr,
			   char *buf)
{
	struct drm_connector *connector = to_drm_connector(device);
	return dsi_display_fod_get(connector, buf);
}

static ssize_t complete_commit_time_show(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	struct drm_connector *connector = to_drm_connector(dev);
	return complete_commit_time_get(connector, buf);
}


static ssize_t thermal_hbm_disabled_store(struct device *device,
			   struct device_attribute *attr,
			   const char *buf, size_t count)
{
	struct drm_connector *connector = to_drm_connector(device);
	char *input_copy, *input_dup = NULL;
	bool thermal_hbm_disabled;
	int ret;

	input_copy = kstrdup(buf, GFP_KERNEL);
	if (!input_copy) {
		DRM_ERROR("can not allocate memory\n");
		ret = -ENOMEM;
		goto exit;
	}
	input_dup = input_copy;
	/* removes leading and trailing whitespace from input_copy */
	input_copy = strim(input_copy);
	ret = kstrtobool(input_copy, &thermal_hbm_disabled);
	if (ret) {
		DRM_ERROR("input buffer conversion failed\n");
		ret = -EAGAIN;
		goto exit_free;
	}

	DRM_INFO("set thermal_hbm_disabled %d\n", thermal_hbm_disabled);
	ret = dsi_display_set_thermal_hbm_disabled(connector, thermal_hbm_disabled);

exit_free:
	kfree(input_dup);
exit:
	return ret ? ret : count;
}

static ssize_t thermal_hbm_disabled_show(struct device *device,
			   struct device_attribute *attr,
			   char *buf)
{
	struct drm_connector *connector = to_drm_connector(device);
	bool thermal_hbm_disabled;

	dsi_display_get_thermal_hbm_disabled(connector, &thermal_hbm_disabled);

	return snprintf(buf, PAGE_SIZE, "%d\n", thermal_hbm_disabled);
}

static ssize_t hw_vsync_info_show(struct device *device,
			   struct device_attribute *attr,
			   char *buf)
{
	struct drm_connector *connector = to_drm_connector(device);
	return dsi_display_get_hw_vsync_info(connector, buf);
}

static DEVICE_ATTR_RW(disp_param);
static DEVICE_ATTR_RW(mipi_reg);
static DEVICE_ATTR_RO(oled_pmic_id);
static DEVICE_ATTR_RO(panel_info);
static DEVICE_ATTR_RO(wp_info);
static DEVICE_ATTR_RO(dynamic_fps);
static DEVICE_ATTR_RW(doze_brightness);
static DEVICE_ATTR_RO(gamma_test);
static DEVICE_ATTR_RO(fod_ui_ready);
static DEVICE_ATTR_RO(smart_fps_value);
static DEVICE_ATTR_RO(complete_commit_time);
static DEVICE_ATTR_RW(thermal_hbm_disabled);
static DEVICE_ATTR_RO(hw_vsync_info);

static struct attribute *connector_dev_attrs[] = {
	&dev_attr_disp_param.attr,
	&dev_attr_mipi_reg.attr,
	&dev_attr_oled_pmic_id.attr,
	&dev_attr_panel_info.attr,
	&dev_attr_wp_info.attr,
	&dev_attr_dynamic_fps.attr,
	&dev_attr_doze_brightness.attr,
	&dev_attr_gamma_test.attr,
	&dev_attr_fod_ui_ready.attr,
	&dev_attr_smart_fps_value.attr,
	&dev_attr_complete_commit_time.attr,
	&dev_attr_thermal_hbm_disabled.attr,
	&dev_attr_hw_vsync_info.attr,
	NULL
};

const struct attribute_group xiaomi_connector_dev_group = {
	.attrs = connector_dev_attrs,
};
