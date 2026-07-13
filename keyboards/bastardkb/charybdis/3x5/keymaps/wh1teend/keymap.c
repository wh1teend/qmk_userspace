// Copyright 2026 wh1teend
// SPDX-License-Identifier: GPL-2.0-or-later

#include "wh1teend.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT(
    BASE_L1,  BASE_R1,
    BASE_L2,  BASE_R2,
    BASE_L3,  BASE_R3,
    BASE_TL, BASE_TR
  ),

  [LAYER_LOWER] = LAYOUT(
    LOWER_L1, LOWER_R1,
    LOWER_L2, LOWER_R2,
    LOWER_L3, LOWER_R3,
    LOWER_TL, LOWER_TR
  ),

  [LAYER_RAISE] = LAYOUT(
    RAISE_L1, RAISE_R1,
    RAISE_L2, RAISE_R2,
    RAISE_L3, RAISE_R3,
    RAISE_TL, RAISE_TR
  ),

  [LAYER_POINTER] = LAYOUT(
    PTR_L1,   PTR_R1,
    PTR_L2,   PTR_R2,
    PTR_L3,   PTR_R3,
    PTR_TL, PTR_TR
  ),
};
// clang-format on
