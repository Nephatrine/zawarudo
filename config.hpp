#ifndef CONFIG_HPP
#define CONFIG_HPP

// C Standard Library
#include <cassert>
#include <cmath>
#include <cstdint>

// C++ STL
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <vector>

// ------------
// User Options
// NOTE: At SPACE_SAVING 2, 12-13 subdivisions will probably consume almost all
// of the RAM available to a consumer PC. Keep that in mind if you want to try
// playing around with higher subdivision counts or higher precision.
//
#define SPACE_SAVING 2
#define SUBDIVIDE_LIMIT 14

// ------
// Macros
//
#ifndef M_PI
#	define M_PI 3.1415926535897932384626433832795
#endif
#define DEG2RAD(d) ((d) * M_PI / 180.0)
#define RAD2DEG(r) ((r) / M_PI * 180.0)

// ----------
// Data Types
// Yes, this is a little insane. This eats up a good chunk of memory though so
// we want to keep that under control.
//
namespace zw
{
#if SPACE_SAVING
	using u8_t = std::uint_least8_t;
	using u16_t = std::uint_least16_t;
	using u32_t = std::uint_least32_t;
	using u64_t = std::uint_least64_t;
#else
	using u8_t = std::uint_fast8_t;
	using u16_t = std::uint_fast16_t;
	using u32_t = std::uint_fast32_t;
	using u64_t = std::uint_fast64_t;
#endif
#if SUBDIVIDE_LIMIT <= 2
	using cell_size_t = u8_t;
#elif SUBDIVIDE_LIMIT <= 6
	using cell_size_t = u16_t;
#elif SUBDIVIDE_LIMIT <= 14
	using cell_size_t = u32_t;
#elif SUBDIVIDE_LIMIT <= 30
	using cell_size_t = u64_t;
	static_assert( sizeof( std::size_t ) >= 8,
	               "SUBDIVIDE_LIMIT does not support values over 14." );
#else
#	error "SUBDIVIDE_LIMIT does not support values over 30."
#endif
	
#if SPACE_SAVING == 2
	using real_t = float;
#else
	using real_t = double;
#endif
	using range_t = std::pair<real_t, real_t>;
}

#endif

