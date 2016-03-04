#ifndef COORD_HPP
#define COORD_HPP

#include "vector.hpp"

namespace zw
{
	struct coord
	{
		// Constructors
		
		coord(): coord( 0, 0 ) {};
		
		coord( const real_t longitude, const real_t latitude,
		       const real_t altitude = 1.0 )
			: lon( longitude ), lat( latitude ), alt( altitude )
		{}
		
		coord( const vector &v )
		{
			lon = std::atan2( v.y, v.x );
			lat = std::atan2( v.z, std::sqrt( v.x * v.x + v.y * v.y ) );
			alt = v.magnitude();
		}
		
		coord( const coord & ) = default;
		
		// Functions
		
		vector vec3() const
		{
			return vector( std::cos( lon ) * std::cos( lat ),
			               std::sin( lon ) * std::cos( lat ), std::sin( lat ) ) * alt;
		}
		
		// Operators
		
		coord &operator=( const coord & ) = default;
		coord operator-() const {return coord( -lon, -lat, alt );}
		
		// Public By Design
		real_t lon, lat, alt;
	};
	
	inline std::ostream &operator<<( std::ostream &s, const coord &c )
	{
		return s << "{" << RAD2DEG( c.lon ) << "," << RAD2DEG( c.lat ) << "," << c.alt
		       << "}";
	}
}

#endif

