#ifndef POINT_HPP
#define POINT_HPP

#include "config.hpp"

namespace zw
{
	struct point
	{
		// Constructors
		
		point(): point( 0, 0 ) {}
		point( const real_t x_, const real_t y_ ): x( x_ ), y( y_ ) {}
		point( const point & ) = default;
		
		// Functions
		
		real_t magnitude() const
		{
			return std::sqrt( x * x + y * y );
		}
		
		point &normalize()
		{
			real_t m = magnitude();
			assert( m != 0 );
			return ( *this ) /= m;
		}
		
		// Operators
		
		point &operator=( const point & ) = default;
		point operator-() const	{return point( -x, -y );}
		
		point &operator+=( const point &p )
		{
			x += p.x;
			y += p.y;
			return *this;
		}
		
		point &operator-=( const point &p )
		{
			x -= p.x;
			y -= p.y;
			return *this;
		}
		
		point &operator*=( const real_t s )
		{
			x *= s;
			y *= s;
			return *this;
		}
		
		point &operator/=( const real_t s )
		{
			x /= s;
			y /= s;
			return *this;
		}
		
		// Public By Design
		real_t x, y;
	};
	
	// More Operators
	inline point operator+( const point &a, const point &b ) {return point( a ) += b;}
	inline point operator-( const point &a, const point &b ) {return point( a ) -= b;}
	inline point operator*( const point &p, const real_t s ) {return point( p ) *= s;}
	inline point operator/( const point &p, const real_t s ) {return point( p ) /= s;}
	
	inline std::ostream &operator<<( std::ostream &s, const point &p )
	{
		return s << "{" << p.x << "," << p.y << "}";
	}
}

#endif

