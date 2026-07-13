// Copyright 2026 wh1teend
// SPDX-License-Identifier: GPL-2.0-or-later

#include "wh1teend.h"

// The 3x6 adds one outer column per hand on top of the shared 3x5 core.

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT(
    KC_LGUI, BASE_L1,   BASE_R1,  KC_RGUI,
    KC_LCTL, BASE_L2,   BASE_R2,  KC_RCTL,
    KC_LSFT, BASE_L3,   BASE_R3,  KC_RSFT,
             BASE_TL,   BASE_TR
  ),

  [LAYER_LOWER] = LAYOUT(
    XXXXXXX, LOWER_L1,  LOWER_R1, XXXXXXX,
    XXXXXXX, LOWER_L2,  LOWER_R2, XXXXXXX,
    XXXXXXX, LOWER_L3,  LOWER_R3, XXXXXXX,
             LOWER_TL,  LOWER_TR
  ),

  [LAYER_RAISE] = LAYOUT(
    XXXXXXX, RAISE_L1,  RAISE_R1, XXXXXXX,
    XXXXXXX, RAISE_L2,  RAISE_R2, XXXXXXX,
    XXXXXXX, RAISE_L3,  RAISE_R3, XXXXXXX,
             RAISE_TL,  RAISE_TR
  ),

  [LAYER_POINTER] = LAYOUT(
    XXXXXXX, PTR_L1,    PTR_R1,   XXXXXXX,
    XXXXXXX, PTR_L2,    PTR_R2,   XXXXXXX,
    XXXXXXX, PTR_L3,    PTR_R3,   XXXXXXX,
             PTR_TL,    PTR_TR
  ),
};
// clang-format on
