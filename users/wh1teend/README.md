# wh1teend userspace

[Русская версия](README.ru.md)

Shared configuration for all Charybdis boards (3x5 / 3x6 / 4x6): layers and
keycodes in `common.h`, behavior in `common.c`, tuning in `config.h`. Each
board's `keymap.c` only holds its key grid.

## Trackball modes

The LED color always shows which mode is active.

| Mode | The ball… | Turn on/off | LED |
|------|-----------|-------------|-----|
| **Pointer** | moves the mouse pointer, accelerating with speed | always on | default |
| **Sniping** | moves the pointer slowly and precisely, no acceleration | hold `SNIPING` · toggle `SNP_TOG` · auto while `PT_Z`/`PT_SLSH` held | 🔴 red |
| **Drag-scroll** | scrolls, sticking to one axis | hold `DRGSCRL` · toggle `DRG_TOG` · tap `DRGT_CRTT` | 🟢 green |
| **Carret** | moves the text cursor with arrow keys; jumps word-wise when sniping is on | hold `CARRETM` · toggle `CRT_TOG` · hold `DRGT_CRTT` | 🔵 azure |

Drag-scroll and carret are mutually exclusive — turning one on turns the
other off.

## Pointer layer

Held via `PT_Z` (Z) or `PT_SLSH` (/). The 3x5 core, identical on every board:

```text
QK_BOOT  DPI_RMOD DPI_MOD  S_D_RMOD S_D_MOD  │  S_D_MOD  S_D_RMOD DPI_MOD  DPI_RMOD QK_BOOT
LGUI     LALT     LCTL     LSFT     DRGT_CRTT│  CRT_TOG  RSFT     RCTL     RALT     RGUI
 · ·     DRGSCRL  SNIPING  DRG_TOG  SNP_TOG  │  SNP_TOG  DRG_TOG  SNIPING  DRGSCRL   · ·
                  BTN2     BTN1     BTN3     │  BTN3     BTN1
```

`DPI_MOD`/`DPI_RMOD` step the pointer DPI up/down, `S_D_MOD`/`S_D_RMOD` do
the same for sniping DPI.

## Custom keycodes

Declared in `common.h`, handled in `common.c`. The web configurator shows
them as unknown codes; assign via the **Any** key using the hex value.
Browser-made layout edits only last until the next flash — layers re-seed
from `keymap.c` on every new build (Argos combos, tap dances and DPI
settings survive).

| Key | Hex (Any) | Behavior |
|-----|-----------|----------|
| `DRGT_CRTT` | `0x7E40` | Tap — toggle drag-scroll · Hold — toggle carret |
| `CARRETM`   | `0x7E41` | Carret while held (not placed on any board — spare) |
| `CRT_TOG`   | `0x7E42` | Toggle carret |

## Tuning

Everything lives in [`config.h`](config.h): uncomment a line, change the
value, then

```sh
qmk userspace-compile
```

and flash. No `EE_CLR` needed — layers re-seed automatically.

### Feature switches

| Define | Default | Enables |
|--------|:-------:|---------|
| `POINTER_ACCEL` | ✅ | speed-based pointer acceleration |
| `DRAGSCROLL_AXIS_LOCK` | ✅ | drag-scroll sticks to the dominant axis |
| `CHARYBDIS_DRAGSCROLL_REVERSE_Y` | ✅ | natural (macOS-style) scroll direction |
| `CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE` | ❌ | pointer layer turns on by itself when the ball moves |

### Common tweaks

- **Pointer too jumpy** → raise `POINTER_ACCEL_DIV` (8 → 12…16)
- **Carret too fast** → raise `CARRET_STEP` (55 → 80…120)
- **Carret repeat too slow** → lower `CARRET_TAP_INTERVAL_MS` (16 → 8)
- **On Linux/Windows** → `#define CARRET_WORD_KEY(arrow) C(arrow)`

### All knobs

| Define | Default | Meaning |
|--------|:-------:|---------|
| **Pointer acceleration** (off while sniping) | | |
| `POINTER_ACCEL_THRESHOLD` | 4 | counts per report where acceleration starts; below it motion is 1:1 |
| `POINTER_ACCEL_DIV` | 8 | counts above threshold per +1×; lower = stronger curve |
| `POINTER_ACCEL_LIMIT` | 4 | maximum multiplier |
| **Carret** | | |
| `CARRET_STEP` | 55 | ball travel per arrow tap; lower = more sensitive |
| `CARRET_STEP_SNIPING` | `×4` | travel per word jump (sniping on) |
| `CARRET_TAP_INTERVAL_MS` | 16 | minimum pause between taps |
| `CARRET_IDLE_TIMEOUT_MS` | 300 | buffer reset after the ball rests; kills phantom taps |
| `CARRET_ACCEL_DIV` | 12 | ball speed shrinks the step, down to ¼; higher = weaker, 0 = off |
| `CARRET_WORD_KEY(arrow)` | `A(arrow)` | word-jump chord |
| `CARRET_REVERSE_X` / `_Y` | off | invert carret axes |
| **Drag-scroll** | | |
| `DRAGSCROLL_AXIS_LOCK_TIMEOUT_MS` | 250 | the locked axis is kept until the ball rests this long |
| **VIA** | | |
| `DYNAMIC_KEYMAP_LAYER_COUNT` | 4 | layers stored in EEPROM; keep ≥ layer count in `common.h` |
