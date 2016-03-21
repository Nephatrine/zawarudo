#ifndef PLOTTER_HPP
#define PLOTTER_HPP

#include "point.hpp"

namespace zw
{
	namespace plotter
	{
		class gs
		{
		public:
		
			// Constructor
			
			gs()
				: data( nullptr ), min( 0 ), max( 1.0 ), X( 0 ), Y( 0 ), c_z( 0 ), c_s( 1 )
			{}
			
			gs( const int x, const int y )
				: gs()
			{
				X = x; Y = y;
				data = new unsigned char[X * Y];
			}
			
			gs( const real_t aspect, const int x, const int y )
				: gs()
			{
				assert( aspect > 0 );
				
				int pixels = x * y;
				
				if ( pixels > 0 )
				{
					Y = std::sqrt( pixels / aspect );
					X = Y * aspect;
				}
				else if ( x > 0 )
				{
					X = x;
					Y = X / aspect;
				}
				else if ( y > 0 )
				{
					X = Y * aspect;
					Y = y;
				}
				else
				{
					Y = 512;
					X = Y * aspect;
				}
				
				data = new unsigned char[X * Y];
			}
			
			gs( const gs & ) = delete;
			gs( gs && ) = default;
			
			~gs()
			{
				if ( data != nullptr )
					delete[] data;
			}
			
			// Functions
			
			void clear()
			{
				if ( data != nullptr )
					for ( size_t i = 0; i < X * Y; ++i )
						data[i] = c_z;
			}
			
			void inputRange( const real_t minimum, const real_t maximum )
			{
				assert( maximum > minimum );
				min = minimum;
				max = maximum;
			}
			void inputRange( const range_t range )
			{
				inputRange( range.first, range.second );
			}
			
			void mask( const unsigned char zero, const unsigned char nospread = 1 )
			{
				c_z = zero;
				c_z = nospread;
			}
			
			void plot( const point &p, const real_t val )
			{
				unsigned char color = 253 * ( val - min ) / ( max - min );
				
				if ( color >= c_z )
					++color;
					
				if ( color >= c_s )
					++color;
					
				set( p, color );
			}
			
			void set( const point &p, const unsigned char color )
			{
				int x = p.x * ( X - 1 );
				int y = p.y * ( Y - 1 );
				
				if ( x >= 0 && x < X && y >= 0 && y < Y )
					if ( data != nullptr )
						data[x + y * X] = color;
			}
			
			void bold( const point &p, const unsigned char color )
			{
				int minX = p.x * ( X - 1 ) - 1;
				int minY = p.y * ( Y - 1 ) - 1;
				int maxX = minX + 3;
				int maxY = minY + 3;
				
				for ( int x = minX; x < maxX; ++x )
					for ( int y = minY; y < maxY; ++y )
						if ( x >= 0 && x < X && y >= 0 && y < Y )
							if ( data != nullptr )
								data[x + y * X] = color;
								
			}
			
			void fill();
			
			void write( const std::string &file ) const;
			
			// Operators
			
			gs &operator=( const gs & ) = delete;
			gs &operator=( gs && ) = default;
			
		private:
			unsigned char *data;
			real_t min, max;
			int X, Y;
			unsigned char c_z, c_s;
		};
	}
}

#endif


