#include <drm/drm_mipi_dsi.h>
#include <video/mipi_display.h>

/**
 * mipi_dsi_dcs_set_display_brightness_bigendian() - sets the brightness value of the
 * display with big endian, high byte to 1st parameter, low byte to 2nd parameter
 * @dsi: DSI peripheral device
 * @brightness: brightness value
 *
 * Return: 0 on success or a negative error code on failure.
 */
int mipi_dsi_dcs_set_display_brightness_big_endian(struct mipi_dsi_device *dsi,
					u16 brightness)
{
	u8 payload[2] = { brightness >> 8, brightness & 0xff};
	ssize_t err;

	err = mipi_dsi_dcs_write(dsi, MIPI_DCS_SET_DISPLAY_BRIGHTNESS,
				 payload, sizeof(payload));
	if (err < 0)
		return err;

	return 0;
}
EXPORT_SYMBOL(mipi_dsi_dcs_set_display_brightness_big_endian);
