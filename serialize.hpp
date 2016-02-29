#ifndef SERIALIZE_HPP
#define SERIALIZE_HPP

// C++ STL
#include <fstream>
#include <string>

namespace serialize
{
	class input
	{
	public:
		input( const std::string &fileName ): fileStream( fileName,
			        std::ifstream::binary ) {}
			        
		template<typename T>
		T read()
		{
			T data;
			fileStream.read( reinterpret_cast<char *>( &data ), sizeof( T ) );
			return data;
		}
		
		template<typename T>
		void read( T &data )
		{
			fileStream.read( reinterpret_cast<char *>( &data ), sizeof( T ) );
		}
		
		template<typename T>
		void read( T *data, unsigned int size )
		{
			fileStream.read( reinterpret_cast<char *>( data ), sizeof( T ) * size );
		}
		
		bool exists() const
		{
			return fileStream.good();
		}
		
		void close()
		{
			fileStream.close();
		}
		
	private:
		std::ifstream fileStream;
	};
	
	class output
	{
	public:
		output( const std::string &fileName ): fileStream( fileName,
			        std::ofstream::binary ) {}
			        
		template<typename T>
		void write( T data )
		{
			fileStream.write( reinterpret_cast<char *>( &data ), sizeof( T ) );
		}
		
		template<typename T>
		void write( T *data, unsigned int size )
		{
			fileStream.write( reinterpret_cast<char *>( data ), sizeof( T ) * size );
		}
		
		void close()
		{
			fileStream.close();
		}
		
	private:
		std::ofstream fileStream;
	};
}

#endif

