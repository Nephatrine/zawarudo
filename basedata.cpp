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

void zw::saveBaseData( const int iterationsCurrent,
                       std::unique_ptr<geodesicData[]> &data, const cell_size_t cellsCurrent )
{
	std::stringstream fileName;
	fileName << "geodesic_" << iterationsCurrent << ".dat";
	serialize::output dataFile( fileName.str() );
	
	dataFile.write( cellsCurrent );
	
	for ( cell_size_t i = 0; i < cellsCurrent; ++i )
	{
		dataFile.write( data[i].link, 6 );
		dataFile.write( data[i].v.x );
		dataFile.write( data[i].v.y );
		dataFile.write( data[i].v.z );
		dataFile.write( data[i].region );
	}
}

