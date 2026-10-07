// SPDX-License-Identifier: GPL-3.0-only
#include "core.h"
#include <string.h>
void midi_reset(midi_parser_t *p) { p->status=0; p->used=0; }
void midi_byte(midi_parser_t *p, uint8_t b) {
 if (b>=0xf8) { if(b==0xff) midi_reset(p); return; } // realtime can interrupt ANY message
 if (b&0x80) { p->used=0; p->status=(b<0xf0)?b:0; return; } // system common/SysEx cancel running status
 if (!p->status) return;
 p->data[p->used++]=b;
 uint8_t kind=p->status&0xf0;
 if(p->used < ((kind==0xc0 || kind==0xd0)?1:2)) return;
 p->used=0;
 if((kind==0x80 || kind==0x90) && p->event)
  p->event(p->ctx,(p->status&15)+1,p->data[0],p->data[1],kind==0x90 && p->data[1]!=0);
}
void drums_hit(drums_t *d,uint8_t note,uint8_t velocity) {
 if(!velocity || velocity>127) return;
 for(size_t i=0;i<sizeof(note_map)/sizeof(note_map[0]);i++) if(note_map[i].note==note) {
  unsigned lane=note_map[i].lane;
#if !NITRO_PRO_DRUMS
  if(lane>=YELLOW_CYMBAL) lane=YELLOW+(lane-YELLOW_CYMBAL);
#endif
  hit_lane_t *s=&d->lane[lane];
  if(s->count==HIT_QUEUE_SIZE) { d->overflow++; return; }
  s->velocity[(s->head+s->count)%HIT_QUEUE_SIZE]=velocity; s->count++; return;
 }
}
void drums_tick(drums_t *d,uint32_t now) {
 for(unsigned i=0;i<LANE_COUNT;i++) {
  hit_lane_t *s=&d->lane[i];
  if(s->phase==1 && s->observed && (uint32_t)(now-s->since)>=HIT_HOLD_US) {
   s->phase=2; s->active=0; s->observed=false;
  }
  if(s->phase==2 && s->observed && (uint32_t)(now-s->since)>=RELEASE_HOLD_US) s->phase=0;
  if(s->phase==0 && s->count) {
   s->active=s->velocity[s->head]; s->head=(s->head+1)%HIT_QUEUE_SIZE; s->count--;
   s->phase=1; s->observed=false;
  }
 }
}
void drums_delivered(drums_t *d,uint32_t now) {
 for(unsigned i=0;i<LANE_COUNT;i++) {
  hit_lane_t *s=&d->lane[i];
  if(s->phase && !s->observed) { s->observed=true; s->since=now; }
 }
}
static uint8_t velocity(uint8_t v) { return (uint8_t)(255u-((unsigned)v*255u/127u)); }
void drums_report(const drums_t *d,uint8_t buttons,uint8_t r[27]) {
 // PS3RockBandDrums_Data_t, explicit bytes avoid compiler-specific bitfield layout.
 memset(r,0,27); r[2]=8; memset(r+3,0x80,4);
 for(unsigned i=19;i<27;i+=2) r[i+1]=2; // little-endian neutral accelerometers 0x0200
 r[1]=((buttons&1)?2:0)|((buttons&2)?1:0);
 static const uint8_t hats[16]={8,0,4,8,6,7,5,8,2,1,3,8,8,8,8,8};
 r[2]=hats[(buttons>>2)&15];
 if(d->lane[KICK].active) r[0]|=0x10;
 if(d->lane[RED].active) { r[0]|=4; r[1]|=4; r[12]=velocity(d->lane[RED].active); }
 static const uint8_t bit[3]={8,1,2}, offset[3]={11,14,13};
 bool up=false,down=false;
 for(unsigned c=0;c<3;c++) {
  uint8_t pad=d->lane[YELLOW+c].active, cym=d->lane[YELLOW_CYMBAL+c].active;
  // Derived from Santroller's current PS3 Rock Band mapping, including shared red-velocity slot.
  if(pad && cym && d->lane[RED].active) continue; // format limitation; documented for optional Pro mode
  if(pad || cym) { r[0]|=bit[c]; r[offset[c]]=velocity(pad?pad:cym); }
  if(pad) r[1]|=4;
  if(cym) { r[1]|=8; if(c==0) up=true; if(c==1) down=true; }
  if(pad && cym) r[12]=velocity(cym);
 }
 if(up && down) { if(d->lane[YELLOW_CYMBAL].active>=d->lane[BLUE_CYMBAL].active) down=false; else up=false; }
 // Cymbal classification takes precedence over physical D-pad while striking.
 if(up) r[2]=0; else if(down) r[2]=4;
}
uint16_t drum_feature(uint8_t id,uint8_t *out,uint16_t requested) {
 static const uint8_t init[8]={0x21,0x26,0x01,0x05,0,0,0,0};
 if(id!=0) return 0;
 uint16_t n=requested<sizeof(init)?requested:sizeof(init); memcpy(out,init,n); return n;
}
