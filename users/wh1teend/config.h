// Copyright 2026 wh1teend
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#ifdef VIA_ENABLE
#    define DYNAMIC_KEYMAP_LAYER_COUNT 4
#endif // VIA_ENABLE

// Double-tap Shift to type one word in caps.
#define DOUBLE_TAP_SHIFT_TURNS_ON_CAPS_WORD

// A tap-hold resolves as hold as soon as another key taps inside it: fast
// Shift+key chords stop misfiring as space+key.
#define PERMISSIVE_HOLD

// LEDs off after 5 minutes of inactivity.
#define RGB_MATRIX_TIMEOUT 300000

#ifdef POINTING_DEVICE_ENABLE
// Native auto pointer layer: uncomment here and set
// POINTING_DEVICE_AUTO_MOUSE_ENABLE = yes in rules.mk.
// #define AUTO_MOUSE_DEFAULT_LAYER 3

/* Drag-scroll */

// Natural (macOS-style) scroll direction. Remove to scroll the other way.
#    define CHARYBDIS_DRAGSCROLL_REVERSE_Y

// Stick to the dominant axis instead of drifting diagonally.
#    define DRAGSCROLL_AXIS_LOCK
// #    define DRAGSCROLL_AXIS_LOCK_TIMEOUT_MS 250

// A flick keeps scrolling after the ball stops; touching the ball ends it.
#    define DRAGSCROLL_INERTIA
// #    define DRAGSCROLL_INERTIA_ENGAGE_MS 20  // coast only if ticks came faster than this
// #    define DRAGSCROLL_INERTIA_DECAY 15      // % the interval grows per coast tick
// #    define DRAGSCROLL_INERTIA_CUTOFF_MS 120 // coast stops below this rate

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
// Word-jump chord is picked by detected host OS; define to force one.
// #    define CARRET_WORD_KEY(arrow) A(arrow) while sniping
// #    define CARRET_REVERSE_X
// #    define CARRET_REVERSE_Y

/* Scrub modes (SCRB_* keys) */

// #    define SCRUB_STEP 80              // travel per tap while scrubbing
// #    define SCRUB_UNDO_KEY G(KC_Z)     // SCRB_HST left  (macOS default)
// #    define SCRUB_REDO_KEY LSG(KC_Z)    // SCRB_HST right (macOS default)
#endif // POINTING_DEVICE_ENABLE
