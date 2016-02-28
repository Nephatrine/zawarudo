#ifndef MONOMAP_HPP
#define MONOMAP_HPP

#include <cassert>
#include <string>

#include "config.hpp"

#include "lib/stb_image_write.h"

namespace zw
{
	template<int X, int Y>
	struct monomap
	{
		monomap()
			: data {0}, min( 0 ), max( 255 ), offset( 64 )
		{}
		
		void setPixel( const real_t lat, const real_t lon, const unsigned char value )
		{
			int x = ( lon + 180.0 ) / 360.0 * ( X - 1 );
			int y = ( 180.0 - ( lat + 90.0 ) ) / 180.0 * ( Y - 1 );
			
			assert( x >= 0 && x < X );
			assert( y >= 0 && y < Y );
			
			std::size_t pixel = x + ( y * X );
			
			assert( pixel < X * Y );
			
			data[pixel] = ( 255.0 - offset ) * ( value - min ) / ( max - min ) + offset;
		}
		
		int traverse( int x )
		{
			return ( x < X / 2 ) ? ( x + X / 2 ) : ( x - X / 2 );
		}
		
		void fill()
		{
			unsigned char newstate[X * Y] = {0};
			int nofills = -1;
			int oldfills = 0;
			int div, tot;
			size_t pixel;
			
			while ( nofills != 0 && nofills != oldfills )
			{
				oldfills = nofills;
				nofills = 0;
				
				for ( int x = 0; x < X; ++x )
					for ( int y = 0; y < Y; ++y )
					{
						pixel = x + y * X;
						
						if ( data[pixel] )
							newstate[pixel] = data[pixel];
						else
						{
							div = 0;
							tot = 0;
							
							if ( x > 0 && data[pixel - 1] )
							{
								tot += data[pixel - 1];
								++div;
							}
							else if ( x == 0 && data[pixel + 511] )
							{
								tot += data[pixel + 511];
								++div;
							}
							
							if ( x < X - 1 && data[pixel + 1] )
							{
								tot += data[pixel + 1];
								++div;
							}
							else if ( x == X - 1 && data[pixel - 511] )
							{
								tot += data[pixel - 511];
								++div;
							}
							
							if ( y > 0 && data[pixel - X] )
							{
								tot += data[pixel - X];
								++div;
							}
							else if ( y == 0 && data[traverse( x )] )
							{
								tot += data[traverse( x )];
								++div;
							}
							
							if ( y < Y - 1 && data[pixel + X] )
							{
								tot += data[pixel + X];
								++div;
							}
							else if ( y == Y - 1 && data[traverse( x ) + y * X] )
							{
								tot += data[traverse( x ) + y * X];
								++div;
							}
							
							if ( div > 0 )
								newstate[pixel] = tot / div;
							else
								++nofills;
						}
					}
					
				for ( std::size_t i = 0; i < X * Y; ++i )
					data[i] = newstate[i];
			}
		}
		
		void write( const std::string &fileName ) const
		{
			stbi_write_png( fileName.c_str(), X, Y, 1,
			                reinterpret_cast<const void *>( data ), 0 );
		}
		
		unsigned char data[X *Y];
		unsigned char min;
		unsigned char max;
		unsigned char offset;
	};
}

#endif

