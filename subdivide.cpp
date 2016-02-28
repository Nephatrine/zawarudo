#include <iostream>
#include <sstream>

#include "basedata.hpp"
#include "monomap.hpp"
#include "ezOptionParser.hpp"

void Usage( ez::ezOptionParser &opt )
{
	std::string usage;
	opt.getUsage( usage );
	std::cout << usage;
}

int main( int argc, const char *argv[] )
{
	using namespace zw;
	
	//
	// Commandline Arguments
	//
	
	ez::ezOptionParser opt;
	opt.overview = "\nCreate base data for truncated icosahedron geodesics.";
	opt.syntax = "subdivide [OPTIONS] [iterations]";
	opt.example = "subdivide 10\n\n";
	opt.footer =
	    "Copyright (C) 2016 Daniel Wolf\nThis program is free and without warranty.\n\n";
	opt.add( "", 0, 0, 0, "Display usage instructions.", "-h", "--help" );
	opt.add( "", 0, 0, 0, "Regenerate data from scratch.", "-f", "--force" );
	opt.parse( argc, argv );
	
	if ( opt.isSet( "-h" ) )
	{
		Usage( opt );
		return 0;
	}
	
	if ( opt.lastArgs.size() < 1 )
	{
		std::cerr << "Expected at least 1 argument." << std::endl;
		Usage( opt );
		return 1;
	}
	
	std::vector<std::string> badOptions;
	
	if ( !opt.gotRequired( badOptions ) )
	{
		for ( size_t i = 0; i < badOptions.size(); ++i )
			std::cerr << "Missing Required Option: " << badOptions[i] << std::endl;
			
		Usage( opt );
		return 1;
	}
	
	if ( !opt.gotExpected( badOptions ) )
	{
		for ( size_t i = 0; i < badOptions.size(); ++i )
			std::cerr << "Unexpected Number of Arguments: " << badOptions[i] << std::endl;
			
		Usage( opt );
		return 1;
	}
	
	//
	// User Options
	//
	
	bool forceRegen = false;
	
	if ( opt.isSet( "-f" ) )
		forceRegen = true;
		
	int iterationsNeeded = -1;
	
	if ( opt.lastArgs.size() > 0 )
		std::istringstream( *opt.lastArgs[0] ) >> iterationsNeeded;
		
	if ( iterationsNeeded < 0 )
	{
		std::cerr << "Cannot produce negative subdivisions." << std::endl;
		return 1;
	}
	
	if ( iterationsNeeded > SUBDIVIDE_LIMIT )
	{
		std::cerr << "Cannot produce >" << SUBDIVIDE_LIMIT << " subdivisions." <<
		          std::endl;
		return 1;
	}
	
	//
	// Check Existing Data
	//
	
	std::unique_ptr<geodesicData[]> geodesic;
	cell_size_t cellsNeeded;
	int iterationsCurrent = loadBaseData( iterationsNeeded, geodesic, cellsNeeded,
	                                      true );
	                                      
	if ( forceRegen )
		iterationsCurrent = -1;
		
	auto cellsCurrent = cellsPerIteration( iterationsCurrent );
	
	if ( iterationsCurrent == iterationsNeeded )
	{
		std::cout << "Data exists. No work is required." << std::endl;
		return 0;
	}
	else
	{
		std::cout << "Requires " << cellsNeeded << " Fields (" << ( sizeof(
		              geodesicData ) * cellsNeeded ) << " Bytes)" << std::endl;
		              
		try
		{
			geodesic = std::unique_ptr<geodesicData[]>( new geodesicData[cellsNeeded] );
		}
		catch ( std::bad_alloc &ba )
		{
			std::cerr << "Unable to allocate required memory." << std::endl;
			std::cerr << "Try a smaller subdivision value.\n" << std::endl;
			throw;
		}
	}
	
	//
	// Build Icosahedron
	//
	
	if ( iterationsCurrent < 0 )
	{
		real_t t = ( 1.0 + std::sqrt( 5.0 ) ) / 2.0;
		real_t d = std::sqrt( 1.0 + std::pow( t, 2.0 ) );
		real_t tau = t / d;
		real_t one = 1 / d;
		
		geodesic[0].v.x = one;
		geodesic[0].v.y = 0;
		geodesic[0].v.z = tau;
		geodesic[1].v.x = -one;
		geodesic[1].v.y = 0;
		geodesic[1].v.z = tau;
		geodesic[8].v.x = one;
		geodesic[8].v.y = 0;
		geodesic[8].v.z = -tau;
		geodesic[11].v.x = -one;
		geodesic[11].v.y = 0;
		geodesic[11].v.z = -tau;
		geodesic[2].v.x = tau;
		geodesic[2].v.y = -one;
		geodesic[2].v.z = 0;
		geodesic[4].v.x = tau;
		geodesic[4].v.y = one;
		geodesic[4].v.z = 0;
		geodesic[6].v.x = -tau;
		geodesic[6].v.y = one;
		geodesic[6].v.z = 0;
		geodesic[7].v.x = -tau;
		geodesic[7].v.y = -one;
		geodesic[7].v.z = 0;
		geodesic[3].v.x = 0;
		geodesic[3].v.y = tau;
		geodesic[3].v.z = one;
		geodesic[5].v.x = 0;
		geodesic[5].v.y = -tau;
		geodesic[5].v.z = one;
		geodesic[9].v.x = 0;
		geodesic[9].v.y = -tau;
		geodesic[9].v.z = -one;
		geodesic[10].v.x = 0;
		geodesic[10].v.y = tau;
		geodesic[10].v.z = -one;
		
		geodesic[0].link[0] = 1;
		geodesic[0].link[1] = 5;
		geodesic[0].link[2] = 2;
		geodesic[0].link[3] = 4;
		geodesic[0].link[4] = 3;
		geodesic[0].link[5] = geodesicData::nolink;
		geodesic[1].link[0] = 0;
		geodesic[1].link[1] = 3;
		geodesic[1].link[2] = 6;
		geodesic[1].link[3] = 7;
		geodesic[1].link[4] = 5;
		geodesic[1].link[5] = geodesicData::nolink;
		geodesic[2].link[0] = 0;
		geodesic[2].link[1] = 5;
		geodesic[2].link[2] = 9;
		geodesic[2].link[3] = 8;
		geodesic[2].link[4] = 4;
		geodesic[2].link[5] = geodesicData::nolink;
		geodesic[3].link[0] = 0;
		geodesic[3].link[1] = 4;
		geodesic[3].link[2] = 10;
		geodesic[3].link[3] = 6;
		geodesic[3].link[4] = 1;
		geodesic[3].link[5] = geodesicData::nolink;
		geodesic[4].link[0] = 0;
		geodesic[4].link[1] = 2;
		geodesic[4].link[2] = 8;
		geodesic[4].link[3] = 10;
		geodesic[4].link[4] = 3;
		geodesic[4].link[5] = geodesicData::nolink;
		geodesic[5].link[0] = 0;
		geodesic[5].link[1] = 1;
		geodesic[5].link[2] = 7;
		geodesic[5].link[3] = 9;
		geodesic[5].link[4] = 2;
		geodesic[5].link[5] = geodesicData::nolink;
		geodesic[6].link[0] = 11;
		geodesic[6].link[1] = 7;
		geodesic[6].link[2] = 1;
		geodesic[6].link[3] = 3;
		geodesic[6].link[4] = 10;
		geodesic[6].link[5] = geodesicData::nolink;
		geodesic[7].link[0] = 11;
		geodesic[7].link[1] = 9;
		geodesic[7].link[2] = 5;
		geodesic[7].link[3] = 1;
		geodesic[7].link[4] = 6;
		geodesic[7].link[5] = geodesicData::nolink;
		geodesic[8].link[0] = 11;
		geodesic[8].link[1] = 10;
		geodesic[8].link[2] = 4;
		geodesic[8].link[3] = 2;
		geodesic[8].link[4] = 9;
		geodesic[8].link[5] = geodesicData::nolink;
		geodesic[9].link[0] = 11;
		geodesic[9].link[1] = 8;
		geodesic[9].link[2] = 2;
		geodesic[9].link[3] = 5;
		geodesic[9].link[4] = 7;
		geodesic[9].link[5] = geodesicData::nolink;
		geodesic[10].link[0] = 11;
		geodesic[10].link[1] = 6;
		geodesic[10].link[2] = 3;
		geodesic[10].link[3] = 4;
		geodesic[10].link[4] = 8;
		geodesic[10].link[5] = geodesicData::nolink;
		geodesic[11].link[0] = 6;
		geodesic[11].link[1] = 10;
		geodesic[11].link[2] = 8;
		geodesic[11].link[3] = 9;
		geodesic[11].link[4] = 7;
		geodesic[11].link[5] = geodesicData::nolink;
		
		for ( int i = 0; i < 12; ++i )
		{
			geodesic[i].v = geodesic[i].v.normalize();
			std::cout << "Point " << i << std::endl;
			std::cout << "   " << geodesic[i].v.latitude() << " \t" <<
			          geodesic[i].v.longitude() << " \t" << geodesic[i].v.magnitude() << std::endl;
			geodesic[i].region = i;
		}
		
		iterationsCurrent = 0;
		cellsCurrent = 12;
	}
	
	//
	// Save Last Iteration Performed
	//
	
	saveBaseData( iterationsCurrent, geodesic, cellsCurrent );
	
	//
	// Write Regional Map
	// Right now this is one of the few ways to visualize the output so far.
	//
	
	monomap<512, 256> mapRegion;
	mapRegion.max = 11;
	
	for ( int i = 0; i < cellsCurrent; ++i )
		mapRegion.setPixel( geodesic[i].v.latitude(), geodesic[i].v.longitude(),
		                    geodesic[i].region );
		                    
	mapRegion.fill();
	mapRegion.write( "regions.png" );
	
	return 0;
}

