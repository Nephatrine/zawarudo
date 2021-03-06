#include "plotter.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "lib/stb_image_write.h"

void zw::plotter::gs::write( const std::string &file ) const
{
	stbi_write_png( file.c_str(), X, Y, 1, reinterpret_cast<const void *>( data ),
	                0 );
}

void zw::plotter::gs::fill()
{
	auto newdata = std::unique_ptr<unsigned char[]>( new unsigned char[X * Y] );
	int unfilled = -1;
	int oldcount = 0;
	int d, t;
	size_t p;
	
	while ( unfilled != oldcount )
	{
		oldcount = unfilled;
		unfilled = 0;
		
		for ( int x = 0; x < X; ++x )
			for ( int y = 0; y < Y; ++y )
			{
				p = x + y * X;
				d = t = 0;
				
				if ( data[p] != c_z )
				{
					newdata[p] = data[p];
					continue;
				}
				
				if ( x > 0 && data[p - 1] != c_z && data[p - 1] != c_s )
				{
					t += data[p - 1];
					++d;
				}
				else if ( x == 0 && data[p + X - 1] != c_z && data[p + X - 1] != c_s )
				{
					t += data[p + X - 1];
					++d;
				}
				
				if ( x < X - 1 && data[p + 1] != c_z && data[p + 1] != c_s )
				{
					t += data[p + 1];
					++d;
				}
				else if ( x == X - 1 && data[p - X + 1] != c_z && data[p - X + 1] != c_s )
				{
					t += data[p - X + 1];
					++d;
				}
				
				if ( y > 0 && data[p - X] != c_z && data[p - X] != c_s )
				{
					t += data[p - X];
					++d;
				}
				
				if ( y < Y - 1 && data[p + X] != c_z && data[p + X] != c_s )
				{
					t += data[p + X];
					++d;
				}
				
				if ( d > 0 )
				{
					newdata[p] = t / d;
					
					while ( newdata[p] == c_z || newdata[p] == c_s )
						newdata[p] -= 1;
				}
				else
				{
					newdata[p] = c_z;
					++unfilled;
				}
			}
			
		for ( int i = 0; i <= p; ++i )
			data[i] = newdata[i];
	}
}

