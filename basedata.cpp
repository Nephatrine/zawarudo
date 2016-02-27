#include <sstream>

#include "basedata.hpp"
#include "serialize.hpp"

int zw::loadBaseData( const int iteration,
                      std::unique_ptr<math::vector[]> &vertices, vertex_size_t &sizeNeeded,
                      const bool pretend )
{
	auto cellsNeeded = cellsPerIteration( iteration );
	
	if ( !pretend )
		assert(	cellsNeeded <= sizeNeeded );
	else
		sizeNeeded = cellsNeeded;
		
	serialize::input fileVertices( "base_vertices.dat" );
	
	if ( fileVertices.exists() )
	{
		auto iterationsCurrent = fileVertices.read<int>();
		
		if ( iterationsCurrent > iteration )
			iterationsCurrent = iteration;
			
		std::stringstream linkFile;
		linkFile << "base_links_" << iterationsCurrent << ".dat";
		serialize::input fileLinks( linkFile.str() );
		
		if ( fileLinks.exists() )
		{
			auto cellsCurrent = fileLinks.read<vertex_size_t>();
			
			if ( cellsCurrent == cellsPerIteration( iterationsCurrent ) )
			{
				if ( !pretend )
				{
					for ( zw::vertex_size_t i = 0; i < cellsCurrent; ++i )
					{
						fileVertices.read( vertices[i].x );
						fileVertices.read( vertices[i].y );
						fileVertices.read( vertices[i].z );
					}
				}
				
				return iterationsCurrent;
			}
		}
	}
	
	return -1;
}

