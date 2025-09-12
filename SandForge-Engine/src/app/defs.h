#pragma once

#include <type_traits>
#include <cmath>

#define RELEASE(x) do { if ((x) != nullptr) { delete (x); (x) = nullptr; } } while(0)

typedef int8_t int8;
typedef std::uint8_t uint8;
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

template<class T>
struct Vec2 {
    T x{}, y{};

    constexpr Vec2() = default;
    constexpr Vec2(T x_, T y_) : x(x_), y(y_) {}

    template<class U>
    constexpr explicit Vec2(const Vec2<U>& o) : x(static_cast<T>(o.x)), y(static_cast<T>(o.y)) {}

    // igualdad
    constexpr bool operator==(const Vec2& r) { return (x == r.x && y == r.y); };
    constexpr Vec2& operator+=(const Vec2& r) { x += r.x; y += r.y; return *this; }
    constexpr Vec2& operator-=(const Vec2& r) { x -= r.x; y -= r.y; return *this; }
    friend constexpr Vec2 operator+(Vec2 a, const Vec2& b) { a += b; return a; }
    friend constexpr Vec2 operator-(Vec2 a, const Vec2& b) { a -= b; return a; }

    template<class Q = T, std::enable_if_t<std::is_arithmetic_v<Q>, int> = 0>
    constexpr Vec2& operator*=(Q s) { x *= s; y *= s; return *this; }
    template<class Q = T, std::enable_if_t<std::is_arithmetic_v<Q>, int> = 0>
    constexpr Vec2& operator/=(Q s) { x /= s; y /= s; return *this; }
    template<class Q = T, std::enable_if_t<std::is_arithmetic_v<Q>, int> = 0>
    friend constexpr Vec2 operator*(Vec2 v, Q s) { v *= s; return v; }
    template<class Q = T, std::enable_if_t<std::is_arithmetic_v<Q>, int> = 0>
    friend constexpr Vec2 operator*(Q s, Vec2 v) { v *= s; return v; }
    template<class Q = T, std::enable_if_t<std::is_arithmetic_v<Q>, int> = 0>
    friend constexpr Vec2 operator/(Vec2 v, Q s) { v /= s; return v; }

};

struct RGBAI {

    uint8 r = 0;
    uint8 g = 0;
    uint8 b = 0;
    uint8 a = 0;
    float i = 0;

};

struct Vertex { float x, y; uint32 rgba; };