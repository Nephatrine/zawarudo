#ifndef PROJECTION_HPP
#define PROJECTION_HPP

#include "coord.hpp"
#include "point.hpp"

namespace zw
{
	real_t sinc( real_t x )
	{
		return x ? ( std::sin( x ) / x ) : 1.0;
	}
	
	namespace projection
	{
		class base
		{
		public:
		
			// Constructors
			
			base()
				: min_( -M_PI, -M_PI / 2.0 ), max_( M_PI, M_PI / 2.0 ), meridian_( 0 )
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
				meridian_ = radians;
			}
			
			void window( const point &min, const point &max )
			{
				min_ = min;
				max_ = max;
			}
			
			void window( const real_t longMin, const real_t latMin, const real_t longMax,
			             const real_t latMax )
			{
				min_ = point( longMin, latMin );
				max_ = point( longMax, latMax );
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
			
			bool valid( const coord &c ) const
			{
				return pc( coord( zm( c.lon ), -c.lat ) ) >= 0;
			}
			
			// Operators
			
			base &operator=( const base & ) = delete;
			base &operator=( base && ) = default;
			
		protected:
			virtual real_t pc( const coord & ) const {return 1.0;}
			
			virtual real_t px( const coord &c ) const {return c.lon;}
			virtual real_t py( const coord &c ) const {return c.lat;}
			
			virtual real_t xn() const {return px( coord( min_.x, 0 ) );}
			virtual real_t xx() const {return px( coord( max_.x, 0 ) );}
			virtual real_t yn() const {return py( coord( 0, -max_.y ) );}
			virtual real_t yx() const {return py( coord( 0, -min_.y ) );}
			
			virtual real_t zm( real_t longitude ) const
			{
				longitude -= meridian_;
				
				while ( longitude <= -M_PI )
					longitude += M_PI * 2.0;
					
				while ( longitude > M_PI )
					longitude -= M_PI * 2.0;
					
				return longitude;
			}
			
			point min_, max_;
			real_t meridian_;
		};
		
		class equirectangular: virtual public base
		{
		public:
		
			// Constructor
			
			equirectangular( const real_t parallel )
				: base(), parallel_( parallel )
			{}
			
			virtual ~equirectangular() {}
			
			// Functions
			
			void parallel( const real_t radians ) {parallel_ = radians;}
			
		protected:
			virtual real_t px( const coord &c ) const
			{
				return c.lon * std::cos( parallel_ );
			}
			
			real_t parallel_;
		};
		
		class mercator: public base
		{
		public:
		
			// Constructor
			
			mercator()
				: base()
			{
				real_t maxlat = 2.0 * std::atan( std::exp( M_PI ) ) - M_PI / 2.0;
				min_.y = -maxlat;
				max_.y = maxlat;
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
				: base(), scale_( scale )
			{}
			
			virtual ~stereographic() {}
			
			// Functions
			
			void scale( const real_t value ) {scale_ = value;}
			
		protected:
			virtual real_t px( const coord &c ) const
			{
				return c.lon / std::sqrt( scale_ );
			}
			
			virtual real_t py( const coord &c ) const
			{
				return ( 1.0 + std::sqrt( scale_ ) / scale_ ) * std::tan( c.lat / 2.0 );
			}
			
			real_t scale_;
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
		
		class equalarea: public equirectangular
		{
		public:
		
			// Constructor
			
			equalarea( const real_t parallel )
				: equirectangular( parallel )
			{}
			
			virtual ~equalarea() {}
			
		protected:
			virtual real_t py( const coord &c ) const
			{
				return std::sin( c.lat ) / std::cos( parallel_ );
			}
		};
		
		class sinusoidal: public base
		{
		public:
		
			// Constructor
			
			sinusoidal()
				: base()
			{}
			
			virtual ~sinusoidal() {}
			
		protected:
			virtual real_t px( const coord &c ) const
			{
				return c.lon * std::cos( c.lat );
			}
		};
		
		class ortelius: public base
		{
		public:
		
			// Constructor
			
			ortelius()
				: base()
			{}
			
			virtual ~ortelius() {}
			
		protected:
			virtual real_t pf( const coord &c ) const
			{
				return 0.5 * ( M_PI * M_PI / 4.0 / std::abs( c.lon ) + std::abs( c.lon ) );
			}
			
			virtual real_t px( const coord &c ) const
			{
				return ( std::abs( c.lon ) - pf( c ) + std::sqrt( pf( c ) * pf(
				             c ) - c.lat * c.lat ) ) * ( c.lon < 0 ? -1 : 1 );
			}
		};
		
		class kavrayskiy: public base
		{
		public:
		
			// Constructor
			
			kavrayskiy()
				: base()
			{}
			
			virtual ~kavrayskiy() {}
			
		protected:
			virtual real_t px( const coord &c ) const
			{
				return 3.0 * c.lon / 2.0 * std::sqrt( 1.0 / 3.0 - std::pow( c.lat / M_PI,
				                                      2.0 ) );
			}
		};
		
		class wagner: public base
		{
		public:
		
			// Constructor
			
			wagner()
				: base()
			{}
			
			virtual ~wagner() {}
			
		protected:
			virtual real_t px( const coord &c ) const
			{
				return c.lon * std::sqrt( 1.0 - 3.0 * std::pow( c.lat / M_PI, 2.0 ) );
			}
		};
		
		class aitoff: virtual public base
		{
		public:
		
			// Constructor
			
			aitoff()
				: base()
			{}
			
			virtual ~aitoff() {}
			
		protected:
			virtual real_t pa( const coord &c ) const
			{
				return std::acos( std::cos( c.lat ) * std::cos( c.lon / 2.0 ) );
			}
			
			virtual real_t px( const coord &c ) const
			{
				return 2.0 * std::cos( c.lat ) * std::sin( c.lon / 2.0 ) / zw::sinc( pa( c ) );
			}
			
			virtual real_t py( const coord &c ) const
			{
				return std::sin( c.lat ) / zw::sinc( pa( c ) );
			}
		};
		
		class hammer: public base
		{
		public:
		
			// Constructor
			
			hammer()
				: base()
			{}
			
			virtual ~hammer() {}
			
		protected:
			virtual real_t px( const coord &c ) const
			{
				return 2.0 * std::sqrt( 2.0 ) * std::cos( c.lat ) * std::sin(
				           c.lon / 2.0 ) / std::sqrt( 1.0 + std::cos( c.lat ) * std::cos( c.lon / 2.0 ) );
			}
			
			virtual real_t py( const coord &c ) const
			{
				return std::sqrt( 2.0 ) * std::sin( c.lat ) / std::sqrt( 1.0 + std::cos(
				            c.lat ) * std::cos( c.lon / 2.0 ) );
			}
		};
		
		class winkel: public aitoff, public equirectangular
		{
		public:
		
			// Contructor
			
			winkel( const real_t parallel )
				: equirectangular( parallel )
			{}
			
			virtual ~winkel() {}
			
		protected:
			virtual real_t px( const coord &c ) const
			{
				return 0.5 * ( aitoff::px( c ) + equirectangular::px( c ) );
			}
			
			virtual real_t py( const coord &c ) const
			{
				return 0.5 * ( aitoff::py( c ) + equirectangular::py( c ) );
			}
		};
		
		class orthographic: public equirectangular
		{
		public:
		
			// Constructor
			
			orthographic( const real_t parallel )
				: equirectangular( parallel )
			{
				min_.x = min_.y;
				max_.x = max_.y;
			}
			
			virtual ~orthographic() {}
			
		protected:
			virtual real_t pc( const coord &c ) const
			{
				return std::sin( parallel_ ) * std::sin( c.lat ) + std::cos(
				           parallel_ ) * std::cos( c.lat ) * std::cos( c.lon );
			}
			
			virtual real_t px( const coord &c ) const
			{
				return std::cos( c.lat ) * std::sin( c.lon );
			}
			
			virtual real_t py( const coord &c ) const
			{
				return std::cos( parallel_ ) * std::sin( c.lat ) - std::sin(
				           parallel_ ) * std::cos( c.lat ) * std::cos( c.lon );
			}
		};
	}
}


#endif

