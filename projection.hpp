#ifndef PROJECTION_HPP
#define PROJECTION_HPP

#include "coord.hpp"
#include "point.hpp"

namespace zw
{
	namespace projection
	{
		class base
		{
		public:
		
			// Constructors
			
			base()
				: min( -M_PI, -M_PI / 2.0 ), max( M_PI, M_PI / 2.0 ), std_m( 0 )
			{}
			
			base( const base & ) = delete;
			base( base && ) = default;
			
			virtual ~base()	{}
			
			//
			// Functions
			//
			
			real_t width() const {return xx() - xn();}
			real_t height() const {return yx() - yn();}
			real_t aspect() const {return width() / height();}
			
			void meridian( const real_t radians )
			{
				std_m = radians;
			}
			
			void window( const point &minimum, const point &maximum )
			{
				min = minimum;
				max = maximum;
			}
			
			point convert( const coord &c ) const
			{
				return point( ( px( coord( zm( c.lon ), -c.lat ) ) - xn() ) / width(),
				              ( py( c ) - yn() ) / height() );
			}
			
			point convert( const vector &v ) const
			{
				return convert( coord( v ) );
			}
			
			// Operators
			
			base &operator=( const base & ) = delete;
			base &operator=( base && ) = default;
			
		protected:
			virtual real_t px( const coord &c ) const {return c.lon;}
			virtual real_t py( const coord &c ) const {return c.lat;}
			
			virtual real_t xn() const {return px( coord( min.x, 0 ) );}
			virtual real_t xx() const {return px( coord( max.x, 0 ) );}
			virtual real_t yn() const {return py( coord( 0, -max.y ) );}
			virtual real_t yx() const {return py( coord( 0, -min.y ) );}
			
			virtual real_t zm( real_t lon ) const
			{
				if ( std_m == 0 )
					return lon;
					
				while ( lon - std_m < -M_PI )
					lon += M_PI * 2.0;
					
				return lon - std_m;
			}
			
			point min, max;
			real_t std_m;
		};
		
		class equirectangular: public base
		{
		public:
		
			// Constructor
			
			equirectangular( const real_t parallel )
				: base(), std_p( parallel )
			{}
			
			virtual ~equirectangular() {}
			
			// Functions
			
			void parallel( const real_t radians ) {std_p = radians;}
			
		protected:
			virtual real_t px( const coord &c ) const
			{
				return c.lon * std::cos( std_p );
			}
			
			real_t std_p;
		};
		
		class mercator: public base
		{
		public:
		
			// Constructor
			
			mercator()
				: base()
			{
				real_t maxlat = 2.0 * std::atan( std::exp( M_PI ) ) - M_PI / 2.0;
				min.y = -maxlat;
				max.y = maxlat;
			}
			
			virtual ~mercator() {}
			
		protected:
			virtual real_t py( const coord &c ) const
			{
				return std::log( std::tan( M_PI / 4.0 + c.lat / 2.0 ) );
			}
		};
		
		class stereographic: public base
		{
		public:
		
			// Constructor
			
			stereographic( const real_t scale )
				: base(), m( scale )
			{}
			
			virtual ~stereographic() {}
			
			// Functions
			
			void scale( const real_t s ) {m = s;}
			
		protected:
			virtual real_t px( const coord &c ) const
			{
				return c.lon / std::sqrt( m );
			}
			
			virtual real_t py( const coord &c ) const
			{
				return ( 1.0 + std::sqrt( m ) / m ) * std::tan( c.lat / 2.0 );
			}
			
			real_t m;
		};
		
		class miller: public base
		{
		public:
		
			// Constructor
			
			miller()
				: base()
			{}
			
			virtual ~miller() {}
			
		protected:
			virtual real_t py( const coord &c ) const
			{
				return 5.0 / 4.0 * std::log( std::tan( M_PI / 4.0 + 2.0 * c.lat / 5.0 ) );
			}
		};
	}
}

#endif

