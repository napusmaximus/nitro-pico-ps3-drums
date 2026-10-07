// SPDX-License-Identifier: GPL-3.0-only
#include "tusb.h"
#ifdef UPSTREAM_DESCRIPTOR
#include "upstream_descriptor.h"
#else
#include "ps3_descriptor.h"
#endif
#include <string.h>
// Same VID/PID/bcdDevice and report descriptor as Santroller RockBandDrums PS3.
const tusb_desc_device_t device_descriptor = {
 .bLength=18,.bDescriptorType=TUSB_DESC_DEVICE,.bcdUSB=0x0200,
 .bDeviceClass=0,.bDeviceSubClass=0,.bDeviceProtocol=0,.bMaxPacketSize0=64,
 .idVendor=0x12ba,.idProduct=0x0210,.bcdDevice=0x0200,
 .iManufacturer=1,.iProduct=2,.iSerialNumber=0,.bNumConfigurations=1
};
const uint8_t report_descriptor[]={TUD_HID_REPORT_DESC_PS3_THIRDPARTY_GAMEPAD()};
const uint8_t configuration_descriptor[]={
 TUD_CONFIG_DESCRIPTOR(1,1,0,TUD_CONFIG_DESC_LEN+TUD_HID_INOUT_DESC_LEN,0,100),
 TUD_HID_INOUT_DESCRIPTOR(0,2,HID_ITF_PROTOCOL_NONE,sizeof(report_descriptor),0x01,0x81,64,1)
};
uint8_t const *tud_descriptor_device_cb(void) {return (const uint8_t *)&device_descriptor;}
uint8_t const *tud_descriptor_configuration_cb(uint8_t i) {(void)i; return configuration_descriptor;}
uint8_t const *tud_hid_descriptor_report_cb(uint8_t i) {(void)i; return report_descriptor;}
uint16_t const *tud_descriptor_string_cb(uint8_t i,uint16_t lang) {
 (void)lang; static uint16_t out[48];
 const char *s=i==1?"Nitro Pico Open Source":i==2?"Rock Band Drums":"";
 if(i==0) {out[0]=0x0304;out[1]=0x0409;return out;}
 if(i>2)return NULL;
 size_t n=strlen(s);for(size_t j=0;j<n;j++)out[j+1]=(uint8_t)s[j];
 out[0]=(uint16_t)(0x0300+2*n+2);return out;
}
#ifdef DESCRIPTOR_DUMP
#include <stdio.h>
int main(void) {
 FILE *f=fopen("device.bin","wb");if(!f)return 1;fwrite(&device_descriptor,1,sizeof(device_descriptor),f);fclose(f);
 f=fopen("configuration.bin","wb");if(!f)return 1;fwrite(configuration_descriptor,1,sizeof(configuration_descriptor),f);fclose(f);
 f=fopen("report.bin","wb");if(!f)return 1;fwrite(report_descriptor,1,sizeof(report_descriptor),f);fclose(f);return 0;
}
#endif
