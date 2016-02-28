#ifndef BASEDATA_HPP
#define BASEDATA_HPP

#include <limits>
#include <memory>

#include "config.hpp"
#include "vector.hpp"

namespace zw
{
	//
	// GCC5 Linux x86_64:
	//
	// SPACE_SAVING 0 - 80 Bytes
	// SPACE_SAVING 1 - 56 Bytes
	// SPACE_SAVING 2 - 40 Bytes
	//
	struct geodesicData
	{
		geodesicData()
			: link {0, 0, 0, 0, 0, 0}, region( 12 )
		{}
		
		cell_size_t link[6];
		math::vector v;
		u8_t region;
		
		static const cell_size_t nolink = std::numeric_limits<cell_size_t>::max();
	};
	
	constexpr cell_size_t cellsPerIterationRecurse( const int iteration,
	        const int target, const cell_size_t cells, const cell_size_t faces )
	{
		return ( iteration == target ) ? cells : cellsPerIterationRecurse(
		           iteration + 1, target, cells + ( faces * 3 ) / 2, faces * 4 );
	}
	
	constexpr cell_size_t cellsPerIteration( const int iteration )
	{
		return ( iteration < 0 ) ? 0 : ( iteration == 0 ) ? 12 :
		       cellsPerIterationRecurse( 0, iteration, 12, 20 );
	}
	
	constexpr int regionalIterations( const int iterations )
	{
		return ( iterations <= 2 ) ? 0 : ( iterations <= 6 ) ? 2 :
		       ( iterations <= 14 ) ? 6 : 14;
	}
	
	int loadBaseData( const int iterationsNeeded,
	                  std::unique_ptr<geodesicData[]> &data, cell_size_t &sizeNeeded,
	                  const bool pretend = false );
}

#endif

