#ifndef ANCHORPOINT_PLATFORM_KEY_H
#define ANCHORPOINT_PLATFORM_KEY_H

#include <cstdint>

namespace ap {

// Platform-independent key codes. Window backends translate native key
// events (e.g. Win32 virtual keys) into these.
enum class Key : std::uint16_t {
    Unknown = 0,

    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

    Escape,
    Space,
    Enter,
    Tab,
    Shift,
    Control,
    Alt,

    Left,
    Right,
    Up,
    Down,

    Count
};

} // namespace ap

#endif //ANCHORPOINT_PLATFORM_KEY_H
