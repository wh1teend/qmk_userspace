# Copyright 2026 wh1teend
# SPDX-License-Identifier: GPL-2.0-or-later

# Sources next to this file are not compiled automatically.
SRC += common.c

CAPS_WORD_ENABLE = yes
REPEAT_KEY_ENABLE = yes
OS_DETECTION_ENABLE = yes

# The auto pointer layer needs no switch here: bk_pointing_device defines
# POINTING_DEVICE_AUTO_MOUSE_ENABLE itself and owns the runtime toggle.
