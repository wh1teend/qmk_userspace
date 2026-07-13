// Copyright 2026 wh1teend
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include QMK_KEYBOARD_H

enum wh1teend_layers {
    LAYER_BASE = 0,
    LAYER_LOWER,
    LAYER_RAISE,
    LAYER_POINTER,
};

#define LOWER MO(LAYER_LOWER)
#define RAISE MO(LAYER_RAISE)
#define PT_Z LT(LAYER_POINTER, KC_Z)
#define PT_SLSH LT(LAYER_POINTER, KC_SLSH)

#define CTL_BSP CTL_T(KC_BSPC)
#define SFT_SPC SFT_T(KC_SPC)
#define GUI_ENT GUI_T(KC_ENT)

// The layers below describe the 3x5 core that every Charybdis shares: three
// rows of five keys per hand, three left thumb keys, two right thumb keys.
// Each keyboard's keymap.c feeds these into its own LAYOUT and supplies the
// keys it has on top of the core (outer columns on 3x6/4x6, number row and
// extra thumbs on 4x6).

// clang-format off
#define BASE_L1     KC_Q,    KC_W,    KC_E,    KC_R,    KC_T
#define BASE_R1     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P
#define BASE_L2     KC_A,    KC_S,    KC_D,    KC_F,    KC_G
#define BASE_R2     KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN
#define BASE_L3     PT_Z,    KC_X,    KC_C,    KC_V,    KC_B
#define BASE_R3     KC_N,    KC_M, KC_COMM,  KC_DOT, PT_SLSH
#define BASE_TL  CTL_BSP, SFT_SPC,   LOWER
#define BASE_TR    RAISE, GUI_ENT

#define LOWER_L1  RM_TOGG, KC_MNXT, KC_MPLY, KC_MPRV, XXXXXXX
#define LOWER_R1  KC_LBRC,    KC_7,    KC_8,    KC_9, KC_RBRC
#define LOWER_L2  KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, XXXXXXX
#define LOWER_R2  KC_PPLS,    KC_4,    KC_5,    KC_6, KC_PMNS
#define LOWER_L3  XXXXXXX, XXXXXXX, XXXXXXX,  EE_CLR, QK_BOOT
#define LOWER_R3  KC_PAST,    KC_1,    KC_2,    KC_3, KC_PSLS
#define LOWER_TL  XXXXXXX, XXXXXXX, _______
#define LOWER_TR  XXXXXXX, _______

#define RAISE_L1  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX
#define RAISE_R1  XXXXXXX, KC_VOLU, KC_MUTE, KC_VOLD, XXXXXXX
#define RAISE_L2  KC_LEFT,   KC_UP, KC_DOWN, KC_RGHT, XXXXXXX
#define RAISE_R2  XXXXXXX, KC_RSFT, KC_RCTL, KC_RALT, KC_RGUI
#define RAISE_L3  KC_HOME, KC_PGUP, KC_PGDN,  KC_END, XXXXXXX
#define RAISE_R3  QK_BOOT,  EE_CLR, XXXXXXX, XXXXXXX, XXXXXXX
#define RAISE_TL  _______, _______, XXXXXXX
#define RAISE_TR  _______, XXXXXXX

#define PTR_L1    QK_BOOT,  EE_CLR, XXXXXXX, DPI_MOD, S_D_MOD
#define PTR_R1    S_D_MOD, DPI_MOD, XXXXXXX,  EE_CLR, QK_BOOT
#define PTR_L2    KC_LGUI, KC_LALT, KC_LCTL, KC_LSFT, XXXXXXX
#define PTR_R2    XXXXXXX, KC_RSFT, KC_RCTL, KC_RALT, KC_RGUI
#define PTR_L3    _______, DRGSCRL, SNIPING, EE_CLR,  QK_BOOT
#define PTR_R3    XXXXXXX, XXXXXXX, SNIPING, DRGSCRL, _______
#define PTR_TL    KC_BTN2, KC_BTN1, KC_BTN3
#define PTR_TR    KC_BTN3, KC_BTN1
// clang-format on
