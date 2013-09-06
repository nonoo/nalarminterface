#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "boardconfig.h"

#include <avr/avr_compiler.h>

#define  USB_DEVICE_VENDOR_ID             USB_VID
#define  USB_DEVICE_PRODUCT_ID            USB_PID
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 50 // Consumption on Vbus line (mA)
#define  USB_DEVICE_ATTR                  (USB_CONFIG_ATTR_BUS_POWERED)
// (USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_BUS_POWERED)
// (USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)

//! USB Device string definitions (Optional)
#define  USB_DEVICE_MANUFACTURE_NAME      "Nonoo"
#define  USB_DEVICE_PRODUCT_NAME          "Alarm Module"
#define  USB_DEVICE_SERIAL_NAME           "000000000001"

//#define USB_DEVICE_LOW_SPEED
//#define USB_DEVICE_HS_SUPPORT

/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
#define  UDC_VBUS_EVENT(b_vbus_high)      naiboard_usb_vbus_action(b_vbus_high)
#define  UDC_SOF_EVENT()                  naiboard_usb_sof_action()
#define  UDC_SUSPEND_EVENT()              naiboard_usb_suspend_action()
#define  UDC_RESUME_EVENT()               naiboard_usb_resume_action()
//! Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature
//#define  UDC_REMOTEWAKEUP_ENABLE()        naiboard_usb_remotewakeup_enable()
//extern void user_callback_remotewakeup_enable(void);
//#define  UDC_REMOTEWAKEUP_DISABLE()       naiboard_usb_remotewakeup_disable()
//extern void user_callback_remotewakeup_disable(void);
//! When a extra string descriptor must be supported
//! other than manufacturer, product and serial string
// #define  UDC_GET_EXTRA_STRING()
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */

/**
 * Configuration of vendor interface
 * @{
 */
//! Interface callback definition
#define UDI_VENDOR_ENABLE_EXT()           naiboard_usb_vendor_enable()
#define UDI_VENDOR_DISABLE_EXT()          naiboard_usb_vendor_disable()
#define UDI_VENDOR_SETUP_OUT_RECEIVED()   naiboard_usb_setup_out_received()
#define UDI_VENDOR_SETUP_IN_RECEIVED()    naiboard_usb_setup_in_received()

//! endpoints size for full speed
//! Note: Disable the endpoints of a type, if size equal 0
#define UDI_VENDOR_EPS_SIZE_INT_FS    64
#define UDI_VENDOR_EPS_SIZE_BULK_FS   0
#define UDI_VENDOR_EPS_SIZE_ISO_FS    0

//! endpoints size for high speed
#define UDI_VENDOR_EPS_SIZE_INT_HS    64
#define UDI_VENDOR_EPS_SIZE_BULK_HS   0
#define UDI_VENDOR_EPS_SIZE_ISO_HS    0

//@}

//@}


/**
 * USB Device Driver Configuration
 * @{
 */

#define UDD_NO_SLEEP_MGR

//! The includes of classes and other headers must be done
//! at the end of this file to avoid compile error
#include <avr/udi_vendor_conf.h>
#include "main.h"

#endif // _CONF_USB_H_
