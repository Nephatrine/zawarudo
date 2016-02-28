#include <sstream>

#include "basedata.hpp"
#include "serialize.hpp"

int zw::loadBaseData( const int iterationsNeeded,
                      std::unique_ptr<geodesicData[]> &data, cell_size_t &sizeNeeded,
                      const bool pretend )
{
	auto cellsNeeded = cellsPerIteration( iterationsNeeded );
	
	if ( !pretend )
		assert(	cellsNeeded <= sizeNeeded );
	else
		sizeNeeded = cellsNeeded;
		
	std::stringstream fileName;
	fileName << "geodesic_" << iterationsNeeded << ".dat";
	serialize::input dataFile( fileName.str() );
	
	if ( dataFile.exists() )
	{
		auto cellsCurrent = dataFile.read<cell_size_t>();
		
		if ( cellsCurrent == cellsNeeded )
		{
			if ( !pretend )
			{
				for ( cell_size_t i = 0; i < cellsCurrent; ++i )
				{
					dataFile.read( data[i].link, 6 );
					dataFile.read( data[i].v.x );
					dataFile.read( data[i].v.y );
					dataFile.read( data[i].v.z );
					dataFile.read( data[i].region );
				}
			}
			
			return iterationsNeeded;
		}
	}
	
	return -1;
}

