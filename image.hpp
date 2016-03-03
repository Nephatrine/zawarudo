#ifndef IMAGE_HPP
#define IMAGE_HPP

// C Standard Library
#include <cassert>
#include <cmath>

// C++ STL
#include <string>
#include <utility>

namespace image
{
	class greyscale
	{
	public:
		greyscale( const int x, const int y )
			: data( nullptr ), X( x ), Y( y ), min( 0 ), max( 1.0 )
		{
			data = new unsigned char[X * Y];
		}
		
		greyscale( const float aspect, const int x, const int y )
			: data( nullptr ), X( x ), Y( y ), min( 0 ), max( 1.0 )
		{
			assert( aspect > 0 );
			
			float pixels = x * y;
			
			if ( pixels > 0 )
			{
				Y = std::sqrt( pixels / aspect );
				X = Y * aspect;
			}
			else if ( x > 0 )
				Y = X / aspect;
			else if ( y > 0 )
				X = Y * aspect;
			else
			{
				Y = 512;
				X = Y * aspect;
			}
			
			data = new unsigned char[X * Y];
		}
		
		~greyscale()
		{
			if ( data != nullptr )
				delete[] data;
		}
		
		void range( const float minimum, const float maximum )
		{
			assert( maximum > minimum );
			min = minimum;
			max = maximum;
		}
		
		void plot( const std::pair<float, float> &point, const float val )
		{
			set( point, 250 * ( val - min ) / ( max - min ) + 5 );
		}
		
		void set( const std::pair<float, float> &point, const unsigned char color )
		{
			int x = point.first * ( X - 1 );
			int y = point.second * ( Y - 1 );
			
			assert( x >= 0 && x < X );
			assert( y >= 0 && y < Y );
			
			if ( data != nullptr )
				data[x + y * X] = color;
		}
		
		void fill();
		
		void write( const std::string &file ) const;
		
	private:
		unsigned char *data;
		int X, Y;
		float min, max;
	};
}

#endif

