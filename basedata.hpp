#ifndef BASEDATA_HPP
#define BASEDATA_HPP

#include <memory>

#include "config.hpp"
#include "vector.hpp"

namespace zw
{

	constexpr vertex_size_t cellsPerIterationRecurse( const int iteration,
	        const int target, const vertex_size_t cells, const vertex_size_t faces )
	{
		return ( iteration == target ) ? cells : cellsPerIterationRecurse(
		           iteration + 1, target, cells + ( faces * 3 ) / 2, faces * 4 );
	}
	
	constexpr vertex_size_t cellsPerIteration( const int iteration )
	{
		return ( iteration < 0 ) ? 0 : ( iteration == 0 ) ? 12 :
		       cellsPerIterationRecurse( 0, iteration, 12, 20 );
	}
	
	constexpr int regionalIterations( const int iterations )
	{
		return ( iterations <= 2 ) ? 0 : ( iterations <= 6 ) ? 2 :
		       ( iterations <= 14 ) ? 6 : 14;
	}
	
	int loadBaseData( const int iteration,
	                  std::unique_ptr<math::vector[]> &vertices, vertex_size_t &sizeNeeded,
	                  const bool pretend = false );
}

#endif

