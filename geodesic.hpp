#ifndef GEODESIC_HPP
#define GEODESIC_HPP

// ZaWarudo Headers
#include "config.hpp"

// Utility Headers
#include "vector.hpp"

// If you change this, make sure to change region_t if needed to fit.
#define REGION_LIMIT 10242

namespace zw
{
	using region_t = u16_t;
	
	struct geoData
	{
		geoData() = default;
		geoData( const geoData & ) = default;
		geoData &operator=( const geoData & ) = default;
		
		cell_size_t prevNeighbor( int spoke )
		{
			if ( spoke == 0 )
				return link[5] == nolink ? link[4] : link[5];
			else
				return link[spoke - 1];
		}
		
		cell_size_t nextNeighbor( int spoke )
		{
			if ( spoke == 5 || link[spoke + 1] == nolink )
				return link[0];
			else
				return link[spoke + 1];
		}
		
		cell_size_t link[6];
		vector v;
		region_t region;
		
		static void subdivide( std::unique_ptr<geoData[]> &data, cell_size_t &extant );
		static void icosahedron( std::unique_ptr<geoData[]> &data,
		                         cell_size_t &extant );
		static bool load( std::unique_ptr<geoData[]> &data, const cell_size_t size,
		                  const std::string &file );
		static void save( std::unique_ptr<geoData[]> &data, const cell_size_t size,
		                  const std::string &file );
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
}

#endif

