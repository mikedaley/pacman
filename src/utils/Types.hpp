#pragma once

#include <cstdint>

namespace pacman {

// Integer type aliases for clarity
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;

// Vector2 for positions and directions
template <typename T>
struct Vec2 {
    T x{};
    T y{};

    constexpr Vec2() = default;
    constexpr Vec2(T x, T y) : x(x), y(y) {}

    constexpr Vec2 operator+(const Vec2& other) const { return {x + other.x, y + other.y}; }
    constexpr Vec2 operator*(T scalar) const { return {x * scalar, y * scalar}; }
};

using Vec2i = Vec2<i32>;
using Vec2f = Vec2<f32>;

// Direction enum for movement
enum class Direction { None, Up, Down, Left, Right };

// Convert direction to vector
constexpr Vec2i directionToVec(Direction dir) {
    switch (dir) {
        case Direction::Up:
            return {0, -1};
        case Direction::Down:
            return {0, 1};
        case Direction::Left:
            return {-1, 0};
        case Direction::Right:
            return {1, 0};
        default:
            return {0, 0};
    }
}

// Get opposite direction
constexpr Direction oppositeDirection(Direction dir) {
    switch (dir) {
        case Direction::Up:
            return Direction::Down;
        case Direction::Down:
            return Direction::Up;
        case Direction::Left:
            return Direction::Right;
        case Direction::Right:
            return Direction::Left;
        default:
            return Direction::None;
    }
}

}  // namespace pacman
