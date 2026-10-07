// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <stdint.h>
#define MIDI_RX_PIN 1
#define DEBUG_TX_PIN 4
#define MIDI_CHANNEL 0 // 0 = omni; otherwise 1..16 (Nitro drums normally channel 10)
#define HIT_HOLD_US 4000u
#define RELEASE_HOLD_US 1000u
#define HIT_QUEUE_SIZE 32
#ifndef NITRO_PRO_DRUMS
#define NITRO_PRO_DRUMS 0
#endif
#ifndef NITRO_DEBUG
#define NITRO_DEBUG 0
#endif
// Active-low switches to GND; internal pull-ups. Start, Select, Up, Down, Left, Right.
static const uint8_t button_pins[6] = {6, 7, 8, 9, 10, 11};
typedef enum { KICK, RED, YELLOW, BLUE, GREEN, YELLOW_CYMBAL, BLUE_CYMBAL, GREEN_CYMBAL, LANE_COUNT } lane_t;
typedef struct { uint8_t note; lane_t lane; } note_map_t;
// Nitro module User Guide v1.2, p38. Edit ONLY this table to remap.
static const note_map_t note_map[] = {
 {36,KICK}, {38,RED}, {40,RED},
 {48,YELLOW}, {50,YELLOW}, {45,BLUE}, {47,BLUE}, {43,GREEN}, {58,GREEN},
 {42,YELLOW_CYMBAL}, {46,YELLOW_CYMBAL}, {23,YELLOW_CYMBAL},
 {51,BLUE_CYMBAL}, {49,GREEN_CYMBAL}, {57,GREEN_CYMBAL}
 // Pedal 44 and splash 21 intentionally ignored. Add entries if desired.
};
