// Copyright 2026 wh1teend
// SPDX-License-Identifier: GPL-2.0-or-later

#include "wh1teend.h"

// The 4x6 adds a number row, one outer column per hand, and three extra thumb
// keys on top of the shared 3x5 core.

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT(
     KC_ESC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,       KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_MINS,
     KC_TAB, BASE_L1,   BASE_R1, KC_BSLS,
    KC_LCTL, BASE_L2,   BASE_R2, KC_QUOT,
    KC_LSFT, BASE_L3,   BASE_R3, KC_RSFT,
             BASE_TL,   BASE_TR,
             KC_LALT, KC_BSPC,     KC_DEL
  ),

  [LAYER_LOWER] = LAYOUT(
    KC_TILD, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,    KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_UNDS,
    XXXXXXX, LOWER_L1,  LOWER_R1, XXXXXXX,
    XXXXXXX, LOWER_L2,  LOWER_R2, XXXXXXX,
    XXXXXXX, LOWER_L3,  LOWER_R3, XXXXXXX,
             LOWER_TL,  LOWER_TR,
             XXXXXXX, XXXXXXX,    XXXXXXX
  ),

  [LAYER_RAISE] = LAYOUT(
     KC_GRV,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,      KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10, KC_F11,
    XXXXXXX, RAISE_L1,  RAISE_R1, XXXXXXX,
    XXXXXXX, RAISE_L2,  RAISE_R2, XXXXXXX,
    XXXXXXX, RAISE_L3,  RAISE_R3, XXXXXXX,
             RAISE_TL,  RAISE_TR,
             XXXXXXX, XXXXXXX,    XXXXXXX
  ),

  [LAYER_POINTER] = LAYOUT(
    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    XXXXXXX, PTR_L1,    PTR_R1,   XXXXXXX,
    XXXXXXX, PTR_L2,    PTR_R2,   XXXXXXX,
    XXXXXXX, PTR_L3,    PTR_R3,   XXXXXXX,
             PTR_TL,    PTR_TR,
             XXXXXXX, XXXXXXX,    XXXXXXX
  ),
};
// clang-format on
