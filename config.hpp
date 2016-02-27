#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <cstdint>

// ------------
// User Options
//
#define SPACE_SAVING 2
#define SUBDIVIDE_LIMIT 14

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
	using vertex_size_t = u8_t;
	using region_size_t = u8_t;
	using cell_size_t = u8_t;
#elif SUBDIVIDE_LIMIT <= 6
	using vertex_size_t = u16_t;
	using region_size_t = u8_t;
	using cell_size_t = u8_t;
#elif SUBDIVIDE_LIMIT <= 14
	using vertex_size_t = u32_t;
	using region_size_t = u16_t;
#	if SUBDIVIDE_LIMIT <= 10
	using cell_size_t = u8_t;
#	else
	using cell_size_t = u16_t;
#	endif
#elif SUBDIVIDE_LIMIT <= 30
	using vertex_size_t = u64_t;
	using region_size_t = u32_t;
#	if SUBDIVIDE_LIMIT <= 18
	using cell_size_t = u8_t;
#	elif SUBDIVIDE_LIMIT <= 22
	using cell_size_t = u16_t;
#	else
	using cell_size_t = u32_t;
#	endif
	static_asset( sizeof( size_t ) >= 8,
	              "SUBDIVIDE_LIMIT does not support values over 14." );
#else
#	error "SUBDIVIDE_LIMIT does not support values over 30."
#endif
	
#define MATH_REAL_T zw::real_t
#if SPACE_SAVING == 2
	using real_t = float;
#else
	using real_t = double;
#endif
}

#endif

