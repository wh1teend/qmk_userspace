// Copyright 2026 wh1teend
// SPDX-License-Identifier: GPL-2.0-or-later

#include "common.h"

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

// Word-jump chord: picked by detected host OS (Option+arrow on macOS,
// Ctrl+arrow elsewhere) unless CARRET_WORD_KEY overrides it in config.h.
#    ifdef OS_DETECTION_ENABLE
#        include "os_detection.h"
#    endif // OS_DETECTION_ENABLE

static uint16_t carret_word_chord(uint16_t arrow) {
#    if defined(CARRET_WORD_KEY)
    return CARRET_WORD_KEY(arrow);
#    elif defined(OS_DETECTION_ENABLE)
    switch (detected_host_os()) {
        case OS_MACOS:
        case OS_IOS:
            return A(arrow);
        default:
            return C(arrow);
    }
#    else
    return A(arrow);
#    endif
}

// App switcher chord: Cmd+Tab on macOS, Alt+Tab elsewhere.
static uint8_t app_switch_mod(void) {
#    ifdef OS_DETECTION_ENABLE
    switch (detected_host_os()) {
        case OS_MACOS:
        case OS_IOS:
            return KC_LGUI;
        default:
            return KC_LALT;
    }
#    else
    return KC_LGUI;
#    endif
}

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

// Scrub modes reuse the carret engine: while a SCRB_* key is held the ball
// taps a different key pair instead of arrows.
#    ifndef SCRUB_STEP
#        define SCRUB_STEP 80
#    endif // !SCRUB_STEP

#    ifndef SCRUB_UNDO_KEY
#        define SCRUB_UNDO_KEY G(KC_Z)
#    endif // !SCRUB_UNDO_KEY

#    ifndef SCRUB_REDO_KEY
#        define SCRUB_REDO_KEY LSG(KC_Z)
#    endif // !SCRUB_REDO_KEY

typedef struct {
    uint16_t right, left, up, down; // 0 = axis disabled
} scrub_map_t;

enum scrub_modes { SCRUB_NONE = 0, SCRUB_TABS, SCRUB_VOLUME, SCRUB_SPACES, SCRUB_HISTORY, SCRUB_APPS };

static const scrub_map_t scrub_maps[] = {
    [SCRUB_TABS]    = {C(KC_PGDN), C(KC_PGUP), 0, 0},
    [SCRUB_VOLUME]  = {0, 0, KC_VOLD, KC_VOLU},
    [SCRUB_SPACES]  = {C(KC_RGHT), C(KC_LEFT), 0, 0},
    [SCRUB_HISTORY] = {SCRUB_REDO_KEY, SCRUB_UNDO_KEY, 0, 0},
    [SCRUB_APPS]    = {KC_TAB, S(KC_TAB), 0, 0}, // the held mod comes from app_switch_mod()
};

static uint8_t scrub_mode = SCRUB_NONE;

static bool     carret_enabled      = false;
static int16_t  carret_buffer_x     = 0;
static int16_t  carret_buffer_y     = 0;
static uint16_t carret_speed        = 0;
static uint16_t carret_tap_timer    = 0;
static uint16_t carret_motion_timer = 0;
static bool     btn1_locked         = false;

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
#        ifdef CAPS_WORD_ENABLE
#            include "caps_word.h"
#        endif // CAPS_WORD_ENABLE

// One color per state. Polled from housekeeping because sniping and
// drag-scroll also change deep inside the keyboard code (keycodes,
// auto-sniping layer) where we have no hook.
static void pointer_mode_rgb_task(void) {
    static uint8_t last_mode = 0;

    uint8_t layer = get_highest_layer(layer_state);
    uint8_t mode = carret_enabled                            ? 3
                   : charybdis_get_pointer_dragscroll_enabled() ? 2
                   : charybdis_get_pointer_sniping_enabled()    ? 1
                   : btn1_locked                                ? 6
#        ifdef CAPS_WORD_ENABLE
                   : is_caps_word_on()                          ? 7
#        endif // CAPS_WORD_ENABLE
                   : layer == LAYER_LOWER                       ? 4
                   : layer == LAYER_RAISE                       ? 5
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
        case 4:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            rgb_matrix_sethsv_noeeprom(HSV_GOLD);
            break;
        case 5:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            rgb_matrix_sethsv_noeeprom(HSV_PURPLE);
            break;
        case 6:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            rgb_matrix_sethsv_noeeprom(HSV_ORANGE);
            break;
        case 7:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            rgb_matrix_sethsv_noeeprom(HSV_WHITE);
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

// The ball engine: one tap per step of travel, at most one tap per interval.
// The dominant enabled axis wins and the other is discarded, so a slightly
// diagonal flick gives a clean single-axis motion; the remainder below one
// step stays in the buffer so slow motion is not swallowed. Carret mode taps
// arrows (word chords while sniping); a held SCRB_* key swaps in its own key
// pair.
static void ball_scrub_task(report_mouse_t *mouse_report) {
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

    bool        sniping = charybdis_get_pointer_sniping_enabled();
    scrub_map_t map;
    uint16_t    step;
    if (scrub_mode != SCRUB_NONE) {
        map  = scrub_maps[scrub_mode];
        step = SCRUB_STEP;
    } else {
        map  = (scrub_map_t){KC_RIGHT, KC_LEFT, KC_UP, KC_DOWN};
        step = sniping ? CARRET_STEP_SNIPING : CARRET_STEP;
    }
#    if CARRET_ACCEL_DIV > 0
    {
        uint16_t divisor = 1 + carret_speed / CARRET_ACCEL_DIV;
        if (divisor > 4) {
            divisor = 4;
        }
        step /= divisor;
    }
#    endif // CARRET_ACCEL_DIV > 0

    bool     has_h      = map.right != 0 || map.left != 0;
    bool     has_v      = map.up != 0 || map.down != 0;
    bool     horizontal = has_h && (!has_v || abs(carret_buffer_x) > abs(carret_buffer_y));
    int16_t *axis       = horizontal ? &carret_buffer_x : &carret_buffer_y;
    int16_t *minor      = horizontal ? &carret_buffer_y : &carret_buffer_x;

    if (abs(*axis) < step) {
        return;
    }

    uint16_t keycode = horizontal ? (*axis > 0 ? map.right : map.left) : (*axis > 0 ? map.up : map.down);
    if (scrub_mode == SCRUB_NONE && sniping) {
        keycode = carret_word_chord(keycode);
    }
    if (keycode != 0) {
        tap_code16(keycode);
    }

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

#    ifdef DRAGSCROLL_INERTIA
// A fast flick keeps scrolling after the ball stops, slowing down until it
// dies out. Touching the ball, however slightly, kills the coast at once.
#        ifndef DRAGSCROLL_INERTIA_ENGAGE_MS
#            define DRAGSCROLL_INERTIA_ENGAGE_MS 20 // coast only if real ticks came faster than this
#        endif // !DRAGSCROLL_INERTIA_ENGAGE_MS

#        ifndef DRAGSCROLL_INERTIA_DECAY
#            define DRAGSCROLL_INERTIA_DECAY 15 // % the tick interval grows per synthetic tick
#        endif // !DRAGSCROLL_INERTIA_DECAY

#        ifndef DRAGSCROLL_INERTIA_CUTOFF_MS
#            define DRAGSCROLL_INERTIA_CUTOFF_MS 120 // coast stops once ticks get slower than this
#        endif // !DRAGSCROLL_INERTIA_CUTOFF_MS

static void dragscroll_inertia_task(report_mouse_t *mouse_report) {
    static uint16_t last_real_timer  = 0;
    static uint16_t real_interval    = 0xFF; // smoothed ms between real scroll ticks
    static uint16_t synth_interval   = 0;    // 0 = not coasting
    static uint16_t last_synth_timer = 0;
    static int8_t   dir_h            = 0;
    static int8_t   dir_v            = 0;

    if (mouse_report->h != 0 || mouse_report->v != 0) {
        // Real tick: learn the rate and direction, stop any coast.
        uint16_t dt = timer_elapsed(last_real_timer);
        if (dt > 0xFF) {
            dt = 0xFF;
        }
        real_interval   = (real_interval * 3 + dt) / 4;
        last_real_timer = timer_read();
        dir_h           = (mouse_report->h > 0) - (mouse_report->h < 0);
        dir_v           = (mouse_report->v > 0) - (mouse_report->v < 0);
        synth_interval  = 0;
        return;
    }

    if (synth_interval == 0) {
        // Input just went quiet: coast only after a genuine flick.
        if (real_interval > DRAGSCROLL_INERTIA_ENGAGE_MS || timer_elapsed(last_real_timer) < real_interval * 2) {
            return;
        }
        synth_interval   = real_interval < 8 ? 8 : real_interval;
        last_synth_timer = timer_read();
        real_interval    = 0xFF; // require a fresh flick for the next coast
    }

    if (timer_elapsed(last_synth_timer) < synth_interval) {
        return;
    }
    mouse_report->h  = dir_h;
    mouse_report->v  = dir_v;
    last_synth_timer = timer_read();
    synth_interval += synth_interval * DRAGSCROLL_INERTIA_DECAY / 100 + 1;
    if (synth_interval > DRAGSCROLL_INERTIA_CUTOFF_MS) {
        synth_interval = 0;
    }
}
#    endif // DRAGSCROLL_INERTIA

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (carret_enabled || scrub_mode != SCRUB_NONE) {
        ball_scrub_task(&mouse_report);
    }
#    ifdef POINTER_ACCEL
    else if (!charybdis_get_pointer_sniping_enabled()) {
        pointer_accel_task(&mouse_report);
    }
#    endif // POINTER_ACCEL

    if (btn1_locked) {
        mouse_report.buttons |= 1; // hold MB1: drag without keeping the key down
    }

#    if defined(DRAGSCROLL_AXIS_LOCK) || defined(DRAGSCROLL_INERTIA)
    if (charybdis_get_pointer_dragscroll_enabled()) {
#        ifdef DRAGSCROLL_AXIS_LOCK
        dragscroll_axis_lock_task(&mouse_report);
#        endif // DRAGSCROLL_AXIS_LOCK
#        ifdef DRAGSCROLL_INERTIA
        dragscroll_inertia_task(&mouse_report);
#        endif // DRAGSCROLL_INERTIA
    }
#    endif // DRAGSCROLL_AXIS_LOCK || DRAGSCROLL_INERTIA

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
    static bool     crt_sel_restore;
    static uint8_t  app_mod_held;

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
        case BTN_LOCK:
            if (record->event.pressed) {
                btn1_locked = !btn1_locked;
            }
            return false;
        case SCRB_TAB ... SCRB_HST:
            scrub_mode = record->event.pressed ? SCRUB_TABS + (keycode - SCRB_TAB) : SCRUB_NONE;
            carret_reset();
            return false;
        case SCRB_APP:
            // The switcher only stays open while its mod is held, so hold it
            // for the whole scrub.
            if (record->event.pressed) {
                app_mod_held = app_switch_mod();
                register_code(app_mod_held);
                scrub_mode = SCRUB_APPS;
            } else {
                scrub_mode = SCRUB_NONE;
                unregister_code(app_mod_held);
            }
            carret_reset();
            return false;
        case CRT_SEL:
            // Selection: carret with Shift held, so the ball extends the
            // text selection. Restores the previous carret state on release.
            if (record->event.pressed) {
                crt_sel_restore = get_pointer_carret_enabled();
                register_code(KC_LSFT);
                set_pointer_carret_enabled(true);
            } else {
                unregister_code(KC_LSFT);
                set_pointer_carret_enabled(crt_sel_restore);
            }
            return false;
#endif // POINTING_DEVICE_ENABLE
    }

    return true;
}

#ifdef POINTING_DEVICE_ENABLE
#    ifdef CHARYBDIS_AUTO_SNIPING_ON_LAYER
layer_state_t layer_state_set_user(layer_state_t state) {
    charybdis_set_pointer_sniping_enabled(layer_state_cmp(state, CHARYBDIS_AUTO_SNIPING_ON_LAYER));
    return state;
}
#    endif // CHARYBDIS_AUTO_SNIPING_ON_LAYER
#endif     // POINTING_DEVICE_ENABLE
