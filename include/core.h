// SPDX-License-Identifier: GPL-3.0-only
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "config.h"
typedef void (*midi_event_fn)(void *, uint8_t, uint8_t, uint8_t, bool);
typedef struct { uint8_t status, used, data[2]; midi_event_fn event; void *ctx; } midi_parser_t;
void midi_byte(midi_parser_t *, uint8_t);
void midi_reset(midi_parser_t *);
typedef struct { uint8_t velocity[HIT_QUEUE_SIZE], head, count, active, phase; uint32_t since; bool observed; } hit_lane_t;
typedef struct { hit_lane_t lane[LANE_COUNT]; uint32_t overflow; } drums_t;
void drums_hit(drums_t *, uint8_t note, uint8_t velocity);
void drums_tick(drums_t *, uint32_t now);
void drums_report(const drums_t *, uint8_t buttons, uint8_t out[27]);
// Called ONLY after interrupt IN transfer completion, with the submitted snapshot.
void drums_delivered(drums_t *, uint32_t now);
uint16_t drum_feature(uint8_t id, uint8_t *out, uint16_t requested);
