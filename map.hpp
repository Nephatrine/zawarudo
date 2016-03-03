#ifndef MAP_HPP
#define MAP_HPP

// Utility Headers
#include "vector.hpp"

#ifndef M_PI
#	define M_PI 3.1415926535897932384626433832795
#endif

#define DEG2RAD(x) ((x) * M_PI / 180.0)
#define RAD2DEG(x) ((x) / M_PI * 180.0)

namespace map
{
	struct geoCoord
	{
		geoCoord() = default;
		geoCoord( const geoCoord & ) = default;
		geoCoord &operator=( const geoCoord & ) = default;
		
		geoCoord( const math::real_t longitude, const math::real_t latitude,
		          const math::real_t elevation = 1.0 )
			: lambda( longitude ), phi( latitude ), h( elevation )
		{}
		
		geoCoord( const math::vector &v )
			: lambda( std::atan2( v.y, v.x ) ), phi( std::atan2( v.z,
			        std::sqrt( v.x * v.x + v.y *v.y ) ) ), h( v.magnitude() )
		{}
		
		math::vector cartesian() const
		{
			return math::vector( std::cos( lambda ) * std::cos( phi ),
			                     std::sin( lambda ) * std::cos( phi ), std::sin( phi ) ) * h;
		}
		
		// longitude, latitude, elevation
		math::real_t lambda, phi, h;
	};
	
	namespace project
	{
		class base
		{
		public:
			base()
				: base( -M_PI, M_PI, -M_PI / 2.0, M_PI / 2.0 )
			{}
			
			base( const math::real_t longMin, const math::real_t longMax,
			      const math::real_t latMin, const math::real_t latMax )
				: lambdaMin( longMin ), lambdaMax( longMax ), phiMin( latMin ), phiMax( latMax )
			{}
			
			virtual ~base() {}
			
			math::real_t width() const {return xMax() - xMin();}
			math::real_t height() const {return yMax() - yMin();}
			float aspect() const {return width() / height();}
			
			std::pair<float, float> point( const geoCoord &coord ) const
			{
				return std::pair<float, float>( ( getX( coord ) - xMin() ) / width(),
				                                ( getY( coord ) - yMin() ) / height() );
			}
			
		protected:
			virtual math::real_t getX( const geoCoord &coord ) const {return coord.lambda;}
			virtual math::real_t getY( const geoCoord &coord ) const {return -coord.phi;}
			
			virtual math::real_t xMin() const {return getX( geoCoord( lambdaMin, 0 ) );}
			virtual math::real_t xMax() const {return getX( geoCoord( lambdaMax, 0 ) );}
			virtual math::real_t yMin() const {return getY( geoCoord( 0, phiMax ) );}
			virtual math::real_t yMax() const {return getY( geoCoord( 0, phiMin ) );}
			
			math::real_t lambdaMin, lambdaMax, phiMin, phiMax;
		};
		
		class equirectangular: public base
		{
		public:
			equirectangular()
				: equirectangular( 0 )
			{}
			
			equirectangular( const math::real_t standard )
				: base(), sp( standard )
			{}
			
			equirectangular( const math::real_t standard, const math::real_t longMin,
			                 const math::real_t longMax, const math::real_t latMin,
			                 const math::real_t latMax )
				: base( longMin, longMax, latMin, latMax ), sp( standard )
			{}
			
			virtual ~equirectangular() {}
			
		protected:
			virtual math::real_t getX( const geoCoord &coord ) const
			{
				return coord.lambda * std::cos( sp );
			}
			
			math::real_t sp;
		};
	}
}

#endif

