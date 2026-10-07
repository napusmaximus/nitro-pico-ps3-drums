// SPDX-License-Identifier: GPL-3.0-only
#include "core.h"
#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/uart.h"
#include "tusb.h"
#include <stdio.h>
#include <string.h>
static drums_t drums;
static midi_parser_t parser;
static queue_t rx;
static volatile uint32_t uart_errors, rx_overflow;
static bool inflight, connected;
static uint8_t last_report[27];
#if NITRO_DEBUG
static char logbuf[2048];static unsigned loghead,logcount;static uint32_t logdrop;
static void log_line(const char *s) {
 size_t n=strlen(s);if(n>sizeof(logbuf)-logcount){logdrop++;return;}
 for(size_t i=0;i<n;i++){logbuf[(loghead+logcount)%sizeof(logbuf)]=s[i];logcount++;}
}
static void debug_task(void) {
 while(logcount && uart_is_writable(uart1)) { uart_get_hw(uart1)->dr=(uint8_t)logbuf[loghead];loghead=(loghead+1)%sizeof(logbuf);logcount--; }
 static uint32_t last;
 uint32_t now=time_us_32();if((uint32_t)(now-last)>=1000000){last=now;char b[160];
 snprintf(b,sizeof b,"stats rx_overflow=%lu uart_errors=%lu hit_overflow=%lu logdrop=%lu\r\n",(unsigned long)rx_overflow,(unsigned long)uart_errors,(unsigned long)drums.overflow,(unsigned long)logdrop);log_line(b);}
}
#endif
static void uart_rx(void) {
 while(uart_is_readable(uart0)) {
  uint32_t raw=uart_get_hw(uart0)->dr;
  if(raw&0xf00u){uart_errors++;uart_get_hw(uart0)->rsr=0;continue;}
  uint8_t b=(uint8_t)raw;if(!queue_try_add(&rx,&b))rx_overflow++;
 }
}
static void event(void *ctx,uint8_t ch,uint8_t note,uint8_t vel,bool on) {
 (void)ctx;
#if NITRO_DEBUG
 char b[80];snprintf(b,sizeof b,"%lu ch=%u note=%u vel=%u %s\r\n",(unsigned long)time_us_32(),ch,note,vel,on?"on":"off");log_line(b);
#endif
 if(connected && on && (MIDI_CHANNEL==0 || ch==MIDI_CHANNEL))drums_hit(&drums,note,vel);
 // Note Off does not cancel an undelivered percussion strike or truncate its minimum hold.
}
static uint8_t buttons(uint32_t now) {
 static uint8_t stable,candidate;static uint32_t changed[6];
 for(unsigned i=0;i<6;i++){uint8_t mask=1u<<i;bool pressed=!gpio_get(button_pins[i]);
 if(pressed!=!!(candidate&mask)){candidate^=mask;changed[i]=now;}
 if((uint32_t)(now-changed[i])>=5000)stable=(stable&~mask)|(candidate&mask);}
 return stable;
}
void tud_hid_report_complete_cb(uint8_t instance,uint8_t const *report,uint16_t len) {
 (void)instance;(void)report;
 if(inflight && len==27)drums_delivered(&drums,time_us_32());
 inflight=false;
}
void tud_hid_report_failed_cb(uint8_t instance,hid_report_type_t type,uint8_t const *report,uint16_t len) {
 (void)instance;(void)report;(void)len;
 // Keep unobserved strikes/releases pending; allow resubmission after a failed IN.
 if(type==HID_REPORT_TYPE_INPUT)inflight=false;
}
uint16_t tud_hid_get_report_cb(uint8_t instance,uint8_t id,hid_report_type_t type,uint8_t *buffer,uint16_t len) {
 (void)instance;
 if(type==HID_REPORT_TYPE_FEATURE)return drum_feature(id,buffer,len);
 if(type==HID_REPORT_TYPE_INPUT && id==0){uint16_t n=len<27?len:27;memcpy(buffer,last_report,n);return n;}
 return 0;
}
void tud_hid_set_report_cb(uint8_t instance,uint8_t id,hid_report_type_t type,uint8_t const *buffer,uint16_t len) {
 // Instrument LED/output requests are accepted; this board has no player LED bank.
 (void)instance;(void)id;(void)type;(void)buffer;(void)len;
}
static void reset_session(void) {
 uint32_t irq=save_and_disable_interrupts();uint8_t byte;
 while(queue_try_remove(&rx,&byte)){}
 restore_interrupts(irq);
 memset(&drums,0,sizeof drums);midi_reset(&parser);inflight=false;
 drums_report(&drums,0,last_report);
}
void tud_umount_cb(void){connected=false;reset_session();}
void tud_mount_cb(void){reset_session();connected=true;}
void tud_suspend_cb(bool remote){(void)remote;connected=false;reset_session();}
void tud_resume_cb(void){reset_session();connected=true;}
int main(void) {
 queue_init(&rx,sizeof(uint8_t),1024);parser.event=event;
 for(unsigned i=0;i<6;i++){gpio_init(button_pins[i]);gpio_set_dir(button_pins[i],GPIO_IN);gpio_pull_up(button_pins[i]);}
#if NITRO_DEBUG
 uart_init(uart1,115200);gpio_set_function(DEBUG_TX_PIN,GPIO_FUNC_UART);
#endif
 uart_init(uart0,31250);gpio_set_function(MIDI_RX_PIN,GPIO_FUNC_UART);
 uart_set_format(uart0,8,1,UART_PARITY_NONE);uart_set_hw_flow(uart0,false,false);uart_set_fifo_enabled(uart0,true);
 irq_set_exclusive_handler(UART0_IRQ,uart_rx);irq_set_enabled(UART0_IRQ,true);uart_set_irq_enables(uart0,true,false);
 reset_session();tud_init(0);
 uint32_t seen_errors=0,seen_overflow=0;
 for(;;){
  tud_task();
  if(connected && !tud_ready()){connected=false;reset_session();}
  if(seen_errors!=uart_errors || seen_overflow!=rx_overflow){
   uint32_t irq=save_and_disable_interrupts();uint8_t b;while(queue_try_remove(&rx,&b)){};
   seen_errors=uart_errors;seen_overflow=rx_overflow;restore_interrupts(irq);midi_reset(&parser);
  }
  uint8_t b;for(unsigned n=0;n<256 && queue_try_remove(&rx,&b);n++) {
   if(seen_errors!=uart_errors || seen_overflow!=rx_overflow)break;
   midi_byte(&parser,b);
  }
  // Freeze phases while a USB snapshot is outstanding. Incoming hits still enter queues.
  if(connected && !inflight && tud_hid_ready()) {
   drums_tick(&drums,time_us_32());drums_report(&drums,buttons(time_us_32()),last_report);
   if(tud_hid_report(0,last_report,sizeof last_report))inflight=true;
  }
#if NITRO_DEBUG
  debug_task();
#endif
 }
}
