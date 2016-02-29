#ifndef VECTOR_HPP
#define VECTOR_HPP

// C Standard Library
#include <cassert>
#include <cmath>

// C++ STL
#include <ostream>

#define DEG2RAD(x) ((x) * 3.1415926535897932384626433832795 / 180.0)
#define RAD2DEG(x) ((x) / 3.1415926535897932384626433832795 * 180.0)
#define PI 3.1415926535897932384626433832795

namespace math
{
#ifdef MATH_REAL_T
	using real_t = MATH_REAL_T;
#else
	using real_t = double;
#endif
	
	struct vector
	{
		// Constructors
		
		vector( const vector & ) = default;
		
		vector()
			: x( 0 ), y( 0 ), z( 0 )
		{}
		
		vector( const real_t x2, const real_t y2, const real_t z2 )
			: x( x2 ), y( y2 ), z( z2 )
		{}
		
		// Functions
		
		real_t cosTheta( const vector &v ) const
		{
			return dotProduct( v ) / ( magnitude() * v.magnitude() );
		}
		
		real_t dotProduct( const vector &v ) const
		{
			return x * v.x + y * v.y + z * v.z;
		}
		
		real_t latitude() const
		{
			return RAD2DEG( std::atan2( z, std::sqrt( x * x + y * y ) ) );
		}
		
		real_t longitude() const
		{
			return RAD2DEG( std::atan2( y, x ) );
		}
		
		real_t magnitude() const
		{
			return std::sqrt( x * x + y * y + z * z );
		}
		
		vector crossProduct( const vector &v ) const
		{
			return vector( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
		}
		
		vector &normalize()
		{
			real_t m = magnitude();
			assert( m != 0 );
			return ( *this ) /= m;
		}
		
		// Operators
		
		vector &operator=( const vector & ) = default;
		
		vector operator-() const
		{
			return vector( -x, -y, -z );
		}
		
		vector &operator+=( const vector &v )
		{
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}
		
		vector &operator-=( const vector &v )
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}
		
		vector &operator*=( const real_t s )
		{
			x *= s;
			y *= s;
			z *= s;
			return *this;
		}
		
		vector &operator/=( const real_t s )
		{
			x /= s;
			y /= s;
			z /= s;
			return *this;
		}
		
		// Public By Design
		real_t x, y, z;
	};
	
	// More Operators
	inline vector operator+( const vector &a, const vector &b ) {return vector( a ) += b;}
	inline vector operator-( const vector &a, const vector &b ) {return vector( a ) -= b;}
	inline vector operator*( const vector &v, const real_t s ) {return vector( v ) *= s;}
	inline vector operator/( const vector &v, const real_t s ) {return vector( v ) /= s;}
	
	// Lat/Long
	inline vector LLC2VEC( const real_t lat, const real_t lon,
	                       const real_t radius = 1.0 )
	{
		return vector( std::cos( DEG2RAD( lat ) ) * std::cos( DEG2RAD( lon ) ),
		               std::cos( DEG2RAD( lat ) ) * std::sin( DEG2RAD( lon ) ),
		               std::sin( DEG2RAD( lat ) ) ) * radius;
	}
	
	inline std::ostream &operator<<( std::ostream &s, const vector &v )
	{
		return s << "{" << v.x << "," << v.y << "," << v.z << "}";
	}
}

#endif

