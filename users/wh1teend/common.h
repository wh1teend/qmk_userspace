// Copyright 2026 wh1teend
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include QMK_KEYBOARD_H

enum charybdis_keymap_layers {
    LAYER_BASE = 0,
    LAYER_LOWER,
    LAYER_RAISE,
    LAYER_POINTER,
};

// Automatically enable sniping-mode on the pointer layer.
#define CHARYBDIS_AUTO_SNIPING_ON_LAYER LAYER_POINTER

enum wh1teend_keycodes {
    // Tap: toggle drag-scroll. Hold: toggle carret mode.
    DRGT_CRTT = SAFE_RANGE,
    // Carret momentary / toggle, mirroring DRGSCRL / DRG_TOG.
    CARRETM,
    CRT_TOG,
    // Carret with Shift held: the ball extends the text selection.
    CRT_SEL,
    // Toggle-hold mouse button 1: drag without keeping the key down.
    BTN_LOCK,
    // Scrub modes: while held, the ball taps keys instead of moving the
    // pointer. Tabs (Ctrl+PgUp/PgDn), volume, macOS spaces (Ctrl+←/→),
    // undo/redo. Keep the order in sync with enum scrub_modes in common.c.
    SCRB_TAB,
    SCRB_VOL,
    SCRB_SPC,
    SCRB_HST,
    // App switcher: holds Cmd/Alt (by OS) while the ball taps Tab.
    SCRB_APP,
};

bool get_pointer_carret_enabled(void);
void set_pointer_carret_enabled(bool enable);

#define LOWER MO(LAYER_LOWER)
#define RAISE MO(LAYER_RAISE)
#define PT_Z LT(LAYER_POINTER, KC_Z)
#define PT_SLSH LT(LAYER_POINTER, KC_SLSH)

#define CTL_BSP CTL_T(KC_BSPC)
#define SFT_SPC SFT_T(KC_SPC)
#define GUI_ENT GUI_T(KC_ENT)

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DRG_TOG KC_NO
#    define DPI_MOD KC_NO
#    define DPI_RMOD KC_NO
#    define S_D_MOD KC_NO
#    define S_D_RMOD KC_NO
#    define SNIPING KC_NO
#    define SNP_TOG KC_NO
#endif // !POINTING_DEVICE_ENABLE
