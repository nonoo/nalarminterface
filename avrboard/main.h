#ifndef MAIN_H_
#define MAIN_H_

bool naiboard_usb_vendor_enable(void);
void naiboard_usb_vendor_disable(void);
bool naiboard_usb_setup_out_received(void);
bool naiboard_usb_setup_in_received(void);
void naiboard_usb_suspend_action(void);
void naiboard_usb_resume_action(void);
void naiboard_usb_sof_action(void);

#endif
