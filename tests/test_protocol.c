// SPDX-License-Identifier: GPL-3.0-only
#include "core.h"
#include "upstream_ps3_drums.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
_Static_assert(sizeof(PS3RockBandDrums_Data_t)==27,"upstream size");
_Static_assert(offsetof(PS3RockBandDrums_Data_t,yellowVelocity)==11,"yellow offset");
_Static_assert(offsetof(PS3RockBandDrums_Data_t,redVelocity)==12,"red offset");
_Static_assert(offsetof(PS3RockBandDrums_Data_t,greenVelocity)==13,"green offset");
_Static_assert(offsetof(PS3RockBandDrums_Data_t,blueVelocity)==14,"blue offset");
int main(void) {
 const uint8_t notes[]={36,38,48,45,43};
 for(unsigned mask=0;mask<32;mask++)for(unsigned v=1;v<128;v++){
  drums_t d={0};uint8_t actual[27];PS3RockBandDrums_Data_t expected={0};
  expected.dpadRight=1; // bit representation of neutral hat=8 after Santroller's hat conversion
  expected.leftStickX=expected.leftStickY=expected.rightStickX=expected.rightStickY=0x80;
  for(unsigned i=0;i<4;i++)expected.unused3[i]=0x200;
  uint8_t value=255-v*255/127;
  if(mask&1)expected.kick1=1;
  if(mask&2){expected.b=1;expected.padFlag=1;expected.redVelocity=value;}
  if(mask&4){expected.y=1;expected.padFlag=1;expected.yellowVelocity=value;}
  if(mask&8){expected.x=1;expected.padFlag=1;expected.blueVelocity=value;}
  if(mask&16){expected.a=1;expected.padFlag=1;expected.greenVelocity=value;}
  for(unsigned i=0;i<5;i++)if(mask&(1u<<i))drums_hit(&d,notes[i],v);
  drums_tick(&d,0);drums_report(&d,0,actual);assert(!memcmp(actual,&expected,27));
 }
 puts("PASS 4064 standard states compared against actual Santroller packed struct");
}
