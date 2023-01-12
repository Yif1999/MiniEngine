#pragma once

#define MiniEngine_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MiniEngine_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MiniEngine_PIN(a, min_value, max_value) MiniEngine_MIN(max_value, MiniEngine_MAX(a, min_value))

#define MiniEngine_VALID_INDEX(idx, range) (((idx) >= 0) && ((idx) < (range)))
#define MiniEngine_PIN_INDEX(idx, range) MiniEngine_PIN(idx, 0, (range)-1)

#define MiniEngine_SIGN(x) ((((x) > 0.0f) ? 1.0f : 0.0f) + (((x) < 0.0f) ? -1.0f : 0.0f))
