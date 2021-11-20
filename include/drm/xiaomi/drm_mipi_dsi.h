#ifndef __XIAOMI_DRM_MIPI_DSI_H__
#define __XIAOMI_DRM_MIPI_DSI_H__

#include <drm/drm_mipi_dsi.h>

int mipi_dsi_dcs_set_display_brightness_big_endian(struct mipi_dsi_device *dsi,
					u16 brightness);

#endif /* __XIAOMI_DRM_MIPI_DSI_H__ */
