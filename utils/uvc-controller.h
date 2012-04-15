/* mostly defines and shit */
#include <stdint.h>

/* these are actually my own! */
#define BASE_CLASS_DEV		0x00
#define BASE_CLASS_COM		0x02
#define BASE_CLASS_HID		0x03
#define BASE_CLASS_PHYS	0x05
#define BASE_CLASS_IMG		0x06
#define BASE_CLASS_CDC		0x0a
#define BASE_CLASS_VID		0x0e
#define BASE_CLASS_MISC	0xef
#define BASE_CLASS_APP		0xfe
#define BASE_CLASS_VEND	0xff
/* only subclass im interested in */
#define SUB_CLASS_VID_CTRL	0x01
/* bmRequestType crap */
#define UVC_RT_TD_SET		0x00		/* bit 7, transf. dir */
#define UVC_RT_TD_GET		0x80		/* 10000000 */
#define UVC_RT_TD_STD		(0x00 << 5)	/* bit 6,5, type */
#define UVC_RT_TY_CLASS	(0x01 << 5)	/* 00100000 */
#define UVC_RT_TY_VENDOR	(0x02 << 5)	/* 01000000 */
#define UVC_RT_TY_RESERVED	(0x03 << 5)	/* 01100000 */
#define UVC_RT_RE_DEVICE	0x00		/* bit 4-0, reciever */
#define UVC_RT_RE_IFACE	0x01		/* interface */
#define UVC_RT_RE_ENDPOINT	0x02
#define UVC_RT_RE_OTHER	0x03	/* rest is reserved */
/* these are the ones used right now: */
#define UVC_BMRT_SET		UVC_RT_TD_SET | UVC_RT_TY_CLASS | UVC_RT_RE_IFACE
#define UVC_BMRT_GET		UVC_RT_TD_GET | UVC_RT_TY_CLASS | UVC_RT_RE_IFACE
/* wIndex tjosan hejsan */
#define UVC_CT_INDEX		(0x01 << 8)
#define UVC_PU_INDEX		(0x02 << 8)
/* most of these are unused but could be used in
 * comming versions.. */
#define RC_UNDEFINED						0x00
#define UVC_SET_CUR							0x01
#define UVC_GET_CUR							0x81
#define UVC_GET_MIN							0x82
#define UVC_GET_MAX							0x83
#define UVC_GET_RES							0x84
#define UVC_GET_LEN							0x85
#define UVC_GET_INFO						0x86
#define UVC_GET_DEF							0x87
#define VC_CONTROL_UNDEFINED				0x00
#define VC_VIDEO_POWER_MODE_CONTROL		0x01
#define VC_REQUEST_ERROR_CODE_CONTROL		0x02
#define CT_CONTROL_UNDEFINED				0x00
#define CT_SCANNING_MODE_CONTROL			(0x01 << 8)
#define CT_AE_MODE_CONTROL					(0x02 << 8)
#define CT_AE_PRIORITY_CONTROL				(0x03 << 8)
#define CT_EXPOSURE_TIME_ABSOLUTE_CONTROL	(0x04 << 8)
#define CT_EXPOSURE_TIME_RELATIVE_CONTROL	(0x05 << 8)
#define CT_FOCUS_ABSOLUTE_CONTROL			(0x06 << 8)
#define CT_FOCUS_RELATIVE_CONTROL			(0x07 << 8)
#define CT_FOCUS_AUTO_CONTROL				(0x08 << 8)
#define CT_IRIS_ABSOLUTE_CONTROL			(0x09 << 8)
#define CT_IRIS_RELATIVE_CONTROL			(0x0A << 8)
#define CT_ZOOM_ABSOLUTE_CONTROL			(0x0B << 8)
#define CT_ZOOM_RELATIVE_CONTROL			(0x0C << 8)
#define CT_PANTILT_ABSOLUTE_CONTROL		(0x0D << 8)
#define CT_PANTILT_RELATIVE_CONTROL		(0x0E << 8)
#define CT_ROLL_ABSOLUTE_CONTROL			(0x0F << 8)
#define CT_ROLL_RELATIVE_CONTROL			(0x10 << 8)
#define CT_PRIVACY_CONTROL					(0x11 << 8)
/* white balance is the only auto feature in the pu */
#define PU_CONTROL_UNDEFINED						(0x00 << 8)
#define PU_BACKLIGHT_COMPENSATION_CONTROL			(0x01 << 8)
#define PU_BRIGHTNESS_CONTROL						(0x02 << 8)
#define PU_CONTRAST_CONTROL						(0x03 << 8)
#define PU_GAIN_CONTROL							(0x04 << 8)
#define PU_POWER_LINE_FREQUENCY_CONTROL			(0x05 << 8)
#define PU_HUE_CONTROL								(0x06 << 8)
#define PU_SATURATION_CONTROL						(0x07 << 8)
#define PU_SHARPNESS_CONTROL						(0x08 << 8)
#define PU_GAMMA_CONTROL							(0x09 << 8)
#define PU_WHITE_BALANCE_TEMPERATURE_CONTROL		(0x0A << 8)
#define PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL	(0x0B << 8)
#define PU_WHITE_BALANCE_COMPONENT_CONTROL			(0x0C << 8)
#define PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL	(0x0D << 8)
#define PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL	(0x0D << 8)
#define PU_DIGITAL_MULTIPLIER_CONTROL				(0x0E << 8)
#define PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL		(0x0F << 8)
#define PU_HUE_AUTO_CONTROL						(0x10 << 8)
#define PU_ANALOG_VIDEO_STANDARD_CONTROL			(0x11 << 8)
#define PU_ANALOG_LOCK_STATUS_CONTROL				(0x12 << 8)

struct ctrlreq
{
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
	unsigned char *data;
};

struct uviface
{	/* this one has to be closed */
	libusb_device_handle *udh;
	uint16_t iface;
};
