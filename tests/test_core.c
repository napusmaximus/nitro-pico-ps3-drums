// SPDX-License-Identifier: GPL-3.0-only
#include "core.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned events,offs;static uint8_t channel,note,vel;static drums_t d;
static void event(void *ctx,uint8_t c,uint8_t n,uint8_t v,bool on) {
 (void)ctx;events++;channel=c;note=n;vel=v;if(on)drums_hit(&d,n,v);else offs++;
}
static void bytes(midi_parser_t *p,const uint8_t *b,size_t n){for(size_t i=0;i<n;i++)midi_byte(p,b[i]);}
#define FEED(...) do {const uint8_t b[]={__VA_ARGS__};bytes(&p,b,sizeof b);}while(0)
static void parser_test(void) {
 midi_parser_t p={.event=event};events=offs=0;memset(&d,0,sizeof d);
 FEED(0x99,38,100,42,127,36,1);assert(events==3 && channel==10 && note==36 && vel==1);
 FEED(38,0,0x89,42,64);assert(events==5 && offs==2);
 FEED(0x99,38,0xf8,100,0xfa,42,0xfe,110);assert(events==7 && note==42);
 FEED(0xf0,1,2,38,0xf8,127,0xf7,38,100);assert(events==7);
 FEED(0x99,38,0xf1,1,100,38,100);assert(events==7);
 FEED(0xc9,1,2,0xd9,127,0x99,38,0x89,38,1);assert(events==8 && offs==3);
 FEED(0x99,38,0xff,100);assert(events==8);
 FEED(0x90,38,127);assert(channel==1 && events==9);
 puts("PASS parser: running status, note off/zero, realtime, SysEx, system common, interrupted messages, channel");
}
static void map_test(void) {
 const uint8_t notes[]={36,38,40,48,50,45,47,43,58,42,46,23,51,49,57};
 const uint8_t bits[]={16,4,4,8,8,1,1,2,2,8,8,8,1,2,2};
 uint8_t r[27];
 for(unsigned i=0;i<sizeof notes;i++){
  memset(&d,0,sizeof d);drums_hit(&d,notes[i],100);drums_tick(&d,0);drums_report(&d,0,r);assert(r[0]==bits[i]);
#if NITRO_PRO_DRUMS
  if(i>=9)assert(r[1]&8);
#else
  assert(!(r[1]&8));assert(r[2]==8);
#endif
 }
 memset(&d,0,sizeof d);drums_hit(&d,0,127);drums_hit(&d,44,127);drums_hit(&d,21,127);drums_hit(&d,38,0);drums_tick(&d,0);drums_report(&d,0,r);assert(!r[0]);
 puts("PASS all mapped notes, cymbals, pedal/splash ignored, unknown notes, zero velocity");
}
static void chords_test(void){uint8_t r[27];memset(&d,0,sizeof d);
 drums_hit(&d,38,100);drums_hit(&d,48,100);drums_hit(&d,36,100);drums_tick(&d,0);drums_report(&d,0,r);assert(r[0]==0x1c);
 memset(&d,0,sizeof d);drums_hit(&d,45,100);drums_hit(&d,43,100);drums_tick(&d,0);drums_report(&d,0,r);assert(r[0]==3);
 puts("PASS simultaneous red+yellow+kick and blue+green");}
static void timing_test(void){uint8_t r[27];memset(&d,0,sizeof d);
 drums_hit(&d,38,80);drums_hit(&d,38,120);drums_tick(&d,0);drums_report(&d,0,r);assert(r[0]==4);
 drums_tick(&d,1000000);assert(d.lane[RED].active==80); // blocked host cannot erase hit
 drums_delivered(&d,1000000);drums_tick(&d,1003999);assert(d.lane[RED].active==80);
 drums_tick(&d,1004000);assert(!d.lane[RED].active);
 drums_tick(&d,2000000);assert(!d.lane[RED].active); // release must ALSO be delivered
 drums_delivered(&d,2000000);drums_tick(&d,2001000);assert(d.lane[RED].active==120);
 memset(&d,0,sizeof d);drums_hit(&d,38,1);drums_tick(&d,UINT32_MAX-2000);drums_delivered(&d,UINT32_MAX-2000);drums_tick(&d,1998);assert(d.lane[RED].active);drums_tick(&d,1999);assert(!d.lane[RED].active);
 puts("PASS repeated hits, USB backpressure, acknowledged release, timer wrap");}
static void load_test(void){uint8_t r[27],last=0;unsigned red=0,blue=0;memset(&d,0,sizeof d);
 for(uint32_t t=0;t<2000000;t+=1000){
  if(t<1000000 && t%10000==0)drums_hit(&d,38,127); // 100 snare strikes/s
  if(t<1000000 && t%10000==5000)drums_hit(&d,45,64);
  drums_tick(&d,t);drums_report(&d,0,r);
  if((r[0]&4)&&!(last&4))red++;
  if((r[0]&1)&&!(last&1))blue++;
  last=r[0];drums_delivered(&d,t);
 }
 assert(red==100 && blue==100 && d.overflow==0);
 memset(&d,0,sizeof d);for(unsigned i=0;i<HIT_QUEUE_SIZE+5;i++)drums_hit(&d,38,100);assert(d.overflow==5);
 puts("PASS 200 alternating strikes, 100 Hz snare roll, bounded queue overflow");}
static void report_test(void){uint8_t r[27];memset(&d,0,sizeof d);drums_report(&d,0,r);
 assert(r[2]==8 && r[3]==128 && r[6]==128 && r[19]==0 && r[20]==2);
 drums_report(&d,1|2|4|32,r);assert(r[1]==3 && r[2]==1);
 drums_report(&d,4|8,r);assert(r[2]==8);
 uint8_t canary[10];memset(canary,0xaa,sizeof canary);assert(drum_feature(0,canary,3)==3 && canary[3]==0xaa);
 assert(drum_feature(0,canary,10)==8 && canary[3]==5 && canary[8]==0xaa);assert(drum_feature(9,canary,10)==0);
#if NITRO_PRO_DRUMS
 drums_hit(&d,42,100);drums_hit(&d,51,80);drums_tick(&d,0);drums_report(&d,0,r);assert(r[0]==9 && r[2]==0 && (r[1]&8));
#endif
 puts("PASS neutral/hat/buttons, bounded feature reply, Pro tie-breaking");}
int main(void){parser_test();map_test();chords_test();timing_test();load_test();report_test();puts("ALL TESTS PASSED");}
