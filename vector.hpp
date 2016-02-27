#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <cassert>
#include <cmath>
#include <ostream>

namespace math
{
#ifdef MATH_REAL_T
	using real_t = MATH_REAL_T;
#else
	using real_t = double;
#endif
	
	class vector
	{
	public:
	
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
		
		real_t magnitude() const
		{
			return std::sqrt( x * x + y * y + z * z );
		}
		
		vector crossProduct( const vector &v ) const
		{
			return vector( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
		}
		
		vector normalize() const
		{
			real_t m = magnitude();
			assert( m != 0 );
			return vector( *this ) /= m;
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
	
	inline std::ostream &operator<<( std::ostream &s, const vector &v )
	{
		return s << "{" << v.x << "," << v.y << "," << v.z << "}";
	}
}

#endif

