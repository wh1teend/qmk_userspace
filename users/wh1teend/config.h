// Copyright 2026 wh1teend
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#ifdef VIA_ENABLE
#    define DYNAMIC_KEYMAP_LAYER_COUNT 4
#endif // VIA_ENABLE

#ifdef POINTING_DEVICE_ENABLE
// Automatically enable the pointer layer when moving the trackball.
// #define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
#    define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS 1000
#    define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD 8

/* Drag-scroll */

// Natural (macOS-style) scroll direction. Remove to scroll the other way.
#    define CHARYBDIS_DRAGSCROLL_REVERSE_Y

// Stick to the dominant axis instead of drifting diagonally.
#    define DRAGSCROLL_AXIS_LOCK
// #    define DRAGSCROLL_AXIS_LOCK_TIMEOUT_MS 250

/* Pointer acceleration (pointer_accel_task in common.c) */

#    define POINTER_ACCEL
// #    define POINTER_ACCEL_THRESHOLD 4 // counts per report where accel starts
// #    define POINTER_ACCEL_DIV 8       // counts above threshold per +1x; lower = stronger
// #    define POINTER_ACCEL_LIMIT 4     // max multiplier

/* Carret mode (carret_task in common.c) */

// #    define CARRET_STEP 55            // travel per arrow tap; lower = more sensitive
// #    define CARRET_STEP_SNIPING (CARRET_STEP * 4) // travel per word jump
// #    define CARRET_TAP_INTERVAL_MS 16 // pacing between taps
// #    define CARRET_IDLE_TIMEOUT_MS 300 // buffer reset after the ball rests
// #    define CARRET_ACCEL_DIV 12       // higher = weaker acceleration; 0 disables
// #    define CARRET_WORD_KEY(arrow) A(arrow) // word-jump chord while sniping
// #    define CARRET_REVERSE_X
// #    define CARRET_REVERSE_Y
#endif // POINTING_DEVICE_ENABLE
