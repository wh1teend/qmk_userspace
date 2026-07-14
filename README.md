# wh1teend QMK userspace

Keymaps and shared behavior for BastardKB Charybdis boards, built against the
[bastardkb-qmk](https://github.com/bastardkb/bastardkb-qmk) fork
(`bkb-procyon` branch).

## Boards

| Board | Keymaps |
|-------|---------|
| Charybdis 3x5 (`splinktegrated_rev1` / `rev3`) | `vendor`, `wh1teend` |
| Charybdis 3x6 | `vendor`, `wh1teend` |
| Charybdis 4x6 | `vendor`, `wh1teend` |

- **`vendor`** — stock BastardKB layout, fully driven by the
  [Argos](https://github.com/Bastardkb/qmk_modules) module (VIA + web config).
- **`wh1teend`** — my layout: 4 shared layers, carret mode, pointer
  acceleration, drag-scroll axis lock, RGB mode indication, plus Argos.
  Documented in [`docs/`](docs) — a Fumadocs site (EN + RU), run with
  `cd docs && pnpm dev`.

## Layout

```text
keyboards/<board>/keymaps/wh1teend/   key grid (keymap.c) + Argos module (keymap.json)
users/wh1teend/                       everything shared: behavior, settings
modules/bastardkb/                    Argos module (git submodule)
docs/                                 documentation site (Fumadocs, EN + RU)
```

A key placement change touches one board's `keymap.c`; a behavior or settings
change in `users/wh1teend/` applies to every board at once.

## Build

One-time setup:

```sh
git clone -b bkb-procyon https://github.com/bastardkb/bastardkb-qmk.git
qmk config user.qmk_home="$(realpath bastardkb-qmk)"
qmk config user.overlay_dir="$(realpath qmk_userspace)"
git submodule update --init --recursive   # inside this repo: Argos module
```

Build everything from `qmk.json`:

```sh
qmk userspace-compile
```

or a single board:

```sh
qmk compile -kb bastardkb/charybdis/4x6 -km wh1teend
```

`.uf2` files land in the `bastardkb-qmk` clone root.

## Flash

Hold the top-inner key while plugging in (or tap `QK_BOOT` — it sits on the
LOWER and pointer layers), then drag the `.uf2` onto the `RPI-RP2` drive.
Flash both halves with the same file.

VIA layers re-seed from the firmware automatically on the first boot of every
new build — no `EE_CLR` dance; Argos combos, tap dances and DPI settings
survive reflashing.

## CI

Every push builds all `qmk.json` targets via GitHub Actions and publishes the
firmware to the Releases page.
