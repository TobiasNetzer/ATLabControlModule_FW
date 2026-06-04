#include "tusb.h"
#include "usb_descriptors.h"


//--------------------------------------------------------------------+
// Device Descriptor
//--------------------------------------------------------------------+

#define USB_VID   0xCafe
#define USB_PID   0x4001
#define USB_BCD   0x0200

static tusb_desc_device_t const desc_device =
{
  .bLength            = sizeof(tusb_desc_device_t),
  .bDescriptorType    = TUSB_DESC_DEVICE,
  .bcdUSB             = USB_BCD,

  .bDeviceClass       = 0x00,
  .bDeviceSubClass    = 0x00,
  .bDeviceProtocol    = 0x00,

  .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

  .idVendor           = USB_VID,
  .idProduct          = USB_PID,
  .bcdDevice          = 0x0100,

  .iManufacturer      = 0x01,
  .iProduct           = 0x02,
  .iSerialNumber      = 0x03,

  .bNumConfigurations = 0x01
};

uint8_t const * tud_descriptor_device_cb(void)
{
  return (uint8_t const *) &desc_device;
}

//--------------------------------------------------------------------+
// HID Report Descriptors
//--------------------------------------------------------------------+

// Device → PC (buttons/state)
static uint8_t const hid_report_in[] =
{
  0x06, 0x00, 0xFF,   // Usage Page (Vendor 0xFF00)
  0x09, 0x01,
  0xA1, 0x01,         // Collection (Application)

  0x15, 0x00,
  0x26, 0xFF, 0x00,

  0x75, 0x08,
  0x95, 0x02,         // 2 bytes (buttons)

  0x09, 0x01,
  0x81, 0x02,         // Input

  0xC0
};

// PC → device (LED commands)
static uint8_t const hid_report_out[] =
{
  0x06, 0x00, 0xFF,   // Vendor page
  0x09, 0x02,
  0xA1, 0x01,

  0x15, 0x00,
  0x26, 0xFF, 0x00,

  0x75, 0x08,        // 8-bit
  0x95, 0x02,        // 2 bytes (LED command)

  0x09, 0x02,
  0x91, 0x02,        // Output

  0xC0
};

uint8_t const * tud_hid_descriptor_report_cb(uint8_t instance)
{
  return (instance == 0) ? hid_report_in : hid_report_out;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

enum
{
  ITF_NUM_HID_IN,
  ITF_NUM_HID_OUT,
  ITF_NUM_TOTAL
};

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + 2*TUD_HID_DESC_LEN)

#define EPNUM_HID_IN   0x81
#define EPNUM_HID_OUT  0x01

uint8_t const desc_configuration[] =
{
  TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0,
                        CONFIG_TOTAL_LEN,
                        TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP,
                        100),

  // HID IN (device → PC)
  TUD_HID_DESCRIPTOR(ITF_NUM_HID_IN,
                     0,
                     HID_ITF_PROTOCOL_NONE,
                     sizeof(hid_report_in),
                     EPNUM_HID_IN,
                     CFG_TUD_HID_EP_BUFSIZE,
                     5),

  // HID OUT (PC → device)
  TUD_HID_DESCRIPTOR(ITF_NUM_HID_OUT,
                     0,
                     HID_ITF_PROTOCOL_NONE,
                     sizeof(hid_report_out),
                     EPNUM_HID_OUT,
                     CFG_TUD_HID_EP_BUFSIZE,
                     5)
};

uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
  (void) index;
  return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

enum
{
  STRID_LANGID = 0,
  STRID_MANUFACTURER,
  STRID_PRODUCT,
  STRID_SERIAL,
};

static char const * string_desc_arr[] =
{
  (const char[]) { 0x09, 0x04 },
  "T_Netzer",
  "ATLabControlModule",
  NULL
};

static uint16_t _desc_str[32];

uint16_t const * tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
  (void) langid;

  size_t chr_count;

  switch (index)
  {
    case STRID_LANGID:
      memcpy(&_desc_str[1], string_desc_arr[0], 2);
      chr_count = 1;
      break;

    case STRID_SERIAL:
	  const char *serial = "0001A2B3C4D5";

	  size_t len = strlen(serial);
	  if (len > 31) len = 31;

	  for (size_t i = 0; i < len; i++) {
		  _desc_str[1 + i] = serial[i];
	  }

	  chr_count = len;

      break;

    default:
      if (index >= sizeof(string_desc_arr)/sizeof(string_desc_arr[0]))
        return NULL;

      const char *str = string_desc_arr[index];
      chr_count = strlen(str);

      if (chr_count > 31) chr_count = 31;

      for (size_t i = 0; i < chr_count; i++)
        _desc_str[1 + i] = str[i];
      break;
  }

  _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);

  return _desc_str;
}
