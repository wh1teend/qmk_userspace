// Copyright 2026 wh1teend
// SPDX-License-Identifier: GPL-2.0-or-later

#include "common.h"

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
#    include "timer.h"

static uint16_t auto_pointer_layer_timer = 0;

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS 1000
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD 8
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#endif     // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#ifdef POINTING_DEVICE_ENABLE
// Carret mode: trackball movement taps the arrow keys instead of moving the
// pointer.

// Trackball travel needed per arrow tap: the caret sensitivity.
#    ifndef CARRET_STEP
#        define CARRET_STEP 55
#    endif // !CARRET_STEP

// With sniping on, the caret jumps word-wise (CARRET_WORD_KEY) instead of
// character-wise, so it gets its own, larger step.
#    ifndef CARRET_STEP_SNIPING
#        define CARRET_STEP_SNIPING (CARRET_STEP * 4)
#    endif // !CARRET_STEP_SNIPING

// Option+arrow: word jumps left/right, paragraph jumps up/down on macOS.
#    ifndef CARRET_WORD_KEY
#        define CARRET_WORD_KEY(arrow) A(arrow)
#    endif // !CARRET_WORD_KEY

// At most one tap per interval: a fast flick drains the buffer as a smooth
// key-repeat instead of one burst the editor renders as a teleport.
#    ifndef CARRET_TAP_INTERVAL_MS
#        define CARRET_TAP_INTERVAL_MS 16
#    endif // !CARRET_TAP_INTERVAL_MS

// Sensor jitter trickles into the buffer; drop it once the ball has been
// still for a while so it never fires a phantom tap.
#    ifndef CARRET_IDLE_TIMEOUT_MS
#        define CARRET_IDLE_TIMEOUT_MS 300
#    endif // !CARRET_IDLE_TIMEOUT_MS

// Caret acceleration: ball speed shrinks the effective step, down to 1/4 of
// it — slow motion stays character-precise, a fast spin skims. Higher value =
// weaker effect; 0 disables.
#    ifndef CARRET_ACCEL_DIV
#        define CARRET_ACCEL_DIV 12
#    endif // !CARRET_ACCEL_DIV

static bool     carret_enabled      = false;
static int16_t  carret_buffer_x     = 0;
static int16_t  carret_buffer_y     = 0;
static uint16_t carret_speed        = 0;
static uint16_t carret_tap_timer    = 0;
static uint16_t carret_motion_timer = 0;

static void carret_reset(void) {
    carret_buffer_x = 0;
    carret_buffer_y = 0;
    carret_speed    = 0;
}

bool get_pointer_carret_enabled(void) {
    return carret_enabled;
}

void set_pointer_carret_enabled(bool enable) {
    if (enable) {
        // The two modes fight over the same trackball; last one wins.
        charybdis_set_pointer_dragscroll_enabled(false);
    } else {
        // Leftover travel must not leak into the next activation.
        carret_reset();
    }
    carret_enabled = enable;
}

#    ifdef RGB_MATRIX_ENABLE
// One color per trackball mode. Polled from housekeeping because sniping and
// drag-scroll also change deep inside the keyboard code (keycodes,
// auto-sniping layer) where we have no hook.
static void pointer_mode_rgb_task(void) {
    static uint8_t last_mode = 0;

    uint8_t mode = carret_enabled                            ? 3
                   : charybdis_get_pointer_dragscroll_enabled() ? 2
                   : charybdis_get_pointer_sniping_enabled()    ? 1
                                                                : 0;
    if (mode == last_mode) {
        return;
    }
    last_mode = mode;

    switch (mode) {
        case 3:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            rgb_matrix_sethsv_noeeprom(HSV_AZURE);
            break;
        case 2:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            rgb_matrix_sethsv_noeeprom(HSV_GREEN);
            break;
        case 1:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            rgb_matrix_sethsv_noeeprom(HSV_RED);
            break;
        default:
            rgb_matrix_reload_from_eeprom();
            break;
    }
}

void housekeeping_task_user(void) {
    pointer_mode_rgb_task();
}
#    endif // RGB_MATRIX_ENABLE

// One tap per step of travel, at most one tap per interval. The dominant
// axis wins and the other is discarded, so a slightly diagonal flick gives a
// clean horizontal or vertical motion; the remainder below one step stays in
// the buffer so slow motion is not swallowed.
static void carret_task(report_mouse_t *mouse_report) {
    if (mouse_report->x != 0 || mouse_report->y != 0) {
        carret_motion_timer = timer_read();
        carret_speed = (carret_speed * 3 + abs(mouse_report->x) + abs(mouse_report->y)) / 4;
#    ifdef CARRET_REVERSE_X
        carret_buffer_x -= mouse_report->x;
#    else
        carret_buffer_x += mouse_report->x;
#    endif
#    ifdef CARRET_REVERSE_Y
        carret_buffer_y -= mouse_report->y;
#    else
        carret_buffer_y += mouse_report->y;
#    endif
        mouse_report->x = 0;
        mouse_report->y = 0;
    } else if (timer_elapsed(carret_motion_timer) > CARRET_IDLE_TIMEOUT_MS) {
        carret_reset();
        return;
    }

    if (timer_elapsed(carret_tap_timer) < CARRET_TAP_INTERVAL_MS) {
        return;
    }

    bool     sniping    = charybdis_get_pointer_sniping_enabled();
    uint16_t step       = sniping ? CARRET_STEP_SNIPING : CARRET_STEP;
#    if CARRET_ACCEL_DIV > 0
    {
        uint16_t divisor = 1 + carret_speed / CARRET_ACCEL_DIV;
        if (divisor > 4) {
            divisor = 4;
        }
        step /= divisor;
    }
#    endif // CARRET_ACCEL_DIV > 0
    bool     horizontal = abs(carret_buffer_x) > abs(carret_buffer_y);
    int16_t *axis       = horizontal ? &carret_buffer_x : &carret_buffer_y;
    int16_t *minor      = horizontal ? &carret_buffer_y : &carret_buffer_x;

    if (abs(*axis) < step) {
        return;
    }

    uint16_t arrow = horizontal ? (*axis > 0 ? KC_RIGHT : KC_LEFT) : (*axis > 0 ? KC_UP : KC_DOWN);
    tap_code16(sniping ? CARRET_WORD_KEY(arrow) : arrow);

    *axis += *axis > 0 ? -step : step;
    *minor           = 0;
    carret_tap_timer = timer_read();
}

#    ifdef POINTER_ACCEL
// Slow motion passes through 1:1 for precision, faster motion is scaled up
// linearly with speed, capped. Disabled while sniping.
#        ifndef POINTER_ACCEL_THRESHOLD
#            define POINTER_ACCEL_THRESHOLD 4 // counts per report where accel starts
#        endif // !POINTER_ACCEL_THRESHOLD

#        ifndef POINTER_ACCEL_DIV
#            define POINTER_ACCEL_DIV 8 // counts above threshold per +1x; lower = stronger
#        endif // !POINTER_ACCEL_DIV

#        ifndef POINTER_ACCEL_LIMIT
#            define POINTER_ACCEL_LIMIT 4 // max multiplier
#        endif // !POINTER_ACCEL_LIMIT

static void pointer_accel_task(report_mouse_t *mouse_report) {
    uint16_t speed = abs(mouse_report->x) + abs(mouse_report->y);
    if (speed <= POINTER_ACCEL_THRESHOLD) {
        return;
    }

    // Fixed point, 64 = 1.0x.
    uint16_t factor = 64 + (speed - POINTER_ACCEL_THRESHOLD) * 64 / POINTER_ACCEL_DIV;
    if (factor > POINTER_ACCEL_LIMIT * 64) {
        factor = POINTER_ACCEL_LIMIT * 64;
    }
    mouse_report->x = CONSTRAIN_HID_XY((int32_t)mouse_report->x * factor / 64);
    mouse_report->y = CONSTRAIN_HID_XY((int32_t)mouse_report->y * factor / 64);
}
#    endif // POINTER_ACCEL

#    ifdef DRAGSCROLL_AXIS_LOCK
// Lock onto the dominant axis and mute the other one until the ball rests,
// so a vertical scroll does not wander sideways.
#        ifndef DRAGSCROLL_AXIS_LOCK_TIMEOUT_MS
#            define DRAGSCROLL_AXIS_LOCK_TIMEOUT_MS 250
#        endif // !DRAGSCROLL_AXIS_LOCK_TIMEOUT_MS

static void dragscroll_axis_lock_task(report_mouse_t *mouse_report) {
    static bool     lock_vertical = true;
    static uint16_t lock_timer    = 0;

    if (mouse_report->h == 0 && mouse_report->v == 0) {
        return;
    }
    if (timer_elapsed(lock_timer) > DRAGSCROLL_AXIS_LOCK_TIMEOUT_MS) {
        lock_vertical = abs(mouse_report->v) >= abs(mouse_report->h);
    }
    lock_timer = timer_read();

    if (lock_vertical) {
        mouse_report->h = 0;
    } else {
        mouse_report->v = 0;
    }
}
#    endif // DRAGSCROLL_AXIS_LOCK

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (carret_enabled) {
        carret_task(&mouse_report);
    }
#    ifdef POINTER_ACCEL
    else if (!charybdis_get_pointer_sniping_enabled()) {
        pointer_accel_task(&mouse_report);
    }
#    endif // POINTER_ACCEL

#    ifdef DRAGSCROLL_AXIS_LOCK
    if (charybdis_get_pointer_dragscroll_enabled()) {
        dragscroll_axis_lock_task(&mouse_report);
    }
#    endif // DRAGSCROLL_AXIS_LOCK

#    ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
    if (abs(mouse_report.x) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD || abs(mouse_report.y) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD) {
        if (auto_pointer_layer_timer == 0) {
            layer_on(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
            rgb_matrix_mode_noeeprom(RGB_MATRIX_NONE);
            rgb_matrix_sethsv_noeeprom(HSV_GREEN);
#        endif // RGB_MATRIX_ENABLE
        }
        auto_pointer_layer_timer = timer_read();
    }
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

    return mouse_report;
}
#endif // POINTING_DEVICE_ENABLE

#ifdef VIA_ENABLE
#    include "dynamic_keymap.h"
#    include "version.h"

// The layout's source of truth is keymap.c, but at runtime VIA serves layers
// from EEPROM, seeded once. VIA itself only re-seeds when the build *date*
// changes, so same-day rebuilds keep serving the stale layout. Track the full
// build timestamp instead and re-seed the layers on every new build; Argos
// combos, tap dances and trackball DPI live in other EEPROM areas and survive.
void keyboard_post_init_user(void) {
    uint32_t build_hash = 5381;
    for (const char *p = QMK_BUILDDATE; *p != '\0'; ++p) {
        build_hash = build_hash * 33 + (uint8_t)*p;
    }
    if (eeconfig_read_user() != build_hash) {
        dynamic_keymap_reset();
        eeconfig_update_user(build_hash);
    }
}
#endif // VIA_ENABLE

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint16_t drgt_crtt_timer;

    switch (keycode) {
#ifdef POINTING_DEVICE_ENABLE
        case DRGT_CRTT:
            if (record->event.pressed) {
                drgt_crtt_timer = timer_read();
            } else if (timer_elapsed(drgt_crtt_timer) < TAPPING_TERM) {
                bool enable = !charybdis_get_pointer_dragscroll_enabled();
                if (enable && get_pointer_carret_enabled()) {
                    set_pointer_carret_enabled(false);
                }
                charybdis_set_pointer_dragscroll_enabled(enable);
            } else {
                set_pointer_carret_enabled(!get_pointer_carret_enabled());
            }
            return false;
        case DRGSCRL:
        case DRG_TOG:
            // Drag-scroll displaces the caret; return true so the keyboard
            // still handles the keycode itself.
            if (record->event.pressed && get_pointer_carret_enabled()) {
                set_pointer_carret_enabled(false);
            }
            return true;
        case CARRETM:
            set_pointer_carret_enabled(record->event.pressed);
            return false;
        case CRT_TOG:
            if (record->event.pressed) {
                set_pointer_carret_enabled(!get_pointer_carret_enabled());
            }
            return false;
#endif // POINTING_DEVICE_ENABLE
    }

    return true;
}

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
void matrix_scan_user(void) {
    if (auto_pointer_layer_timer != 0 && TIMER_DIFF_16(timer_read(), auto_pointer_layer_timer) >= CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS) {
        auto_pointer_layer_timer = 0;
        layer_off(LAYER_POINTER);
#    ifdef RGB_MATRIX_ENABLE
        rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
#    endif // RGB_MATRIX_ENABLE
    }
}
#endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#ifdef POINTING_DEVICE_ENABLE
#    ifdef CHARYBDIS_AUTO_SNIPING_ON_LAYER
layer_state_t layer_state_set_user(layer_state_t state) {
    charybdis_set_pointer_sniping_enabled(layer_state_cmp(state, CHARYBDIS_AUTO_SNIPING_ON_LAYER));
    return state;
}
#    endif // CHARYBDIS_AUTO_SNIPING_ON_LAYER
#endif     // POINTING_DEVICE_ENABLE
