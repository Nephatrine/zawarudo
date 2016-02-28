#include <iostream>
#include <sstream>

#include "basedata.hpp"
#include "ezOptionParser.hpp"

void Usage( ez::ezOptionParser &opt )
{
	std::string usage;
	opt.getUsage( usage );
	std::cout << usage;
}

int main( int argc, const char *argv[] )
{
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

	if ( opt.isSet( "-f" ) )                                                                        		forceRegen = true;

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
	// Load Existing Data
	//
	
	std::unique_ptr<math::vector[]> vertices;
	zw::vertex_size_t cellsNeeded;
	int iterationsCurrent = -1;
	
	if ( !forceRegen )
		iterationsCurrent = zw::loadBaseData( iterationsNeeded, vertices, cellsNeeded,
		                                      true );
	else
		zw::loadBaseData( iterationsNeeded, vertices, cellsNeeded, true );
		                                      
	auto cellsCurrent = zw::cellsPerIteration( iterationsCurrent );
	
	if ( iterationsCurrent == iterationsNeeded )
	{
		std::cout << "Data exists. No work is required." << std::endl;
		return 0;
	}
	else
	{
		std::cout << "Requires " << cellsNeeded << " Verts (" << ( sizeof(
		              math::vector ) * cellsNeeded ) << " Bytes)" << std::endl;
		              
		try
		{
			vertices = std::unique_ptr<math::vector[]>( new math::vector[cellsNeeded] );
		}
		catch ( std::bad_alloc &ba )
		{
			std::cerr << "Unable to allocate required memory." << std::endl;
			std::cerr << "Try a smaller subdivision value.\n" << std::endl;
			throw;
		}
		
		if ( iterationsCurrent >= 0 )
		{
			std::cout << "Loading data for iteration " << iterationsCurrent << "." << std::endl;
			zw::loadBaseData( iterationsNeeded, vertices, cellsNeeded );
		}
	}
	
	//
	// Build Icosahedron
	//
	
	if ( iterationsCurrent < 0 )
	{
		zw::real_t t = ( 1.0 + std::sqrt( 5.0 ) ) / 2.0;
		zw::real_t d = std::sqrt( 1.0 + std::pow( t, 2.0 ) );
		zw::real_t tau = t / d;
		zw::real_t one = 1 / d;
		
		vertices[0].x = one;
		vertices[0].z = tau;
		vertices[1].x = -one;
		vertices[1].z = tau;
		vertices[8].x = one;
		vertices[8].z = -tau;
		vertices[11].x = -one;
		vertices[11].z = -tau;
		vertices[2].x = tau;
		vertices[2].y = -one;
		vertices[4].x = tau;
		vertices[4].y = one;
		vertices[6].x = -tau;
		vertices[6].y = one;
		vertices[7].x = -tau;
		vertices[7].y = -one;
		vertices[3].y = tau;
		vertices[3].z = one;
		vertices[5].y = -tau;
		vertices[5].z = one;
		vertices[9].y = -tau;
		vertices[9].z = -one;
		vertices[10].y = tau;
		vertices[10].z = -one;
		
		iterationsCurrent = 0;
		cellsCurrent = 12;
	}
	
	return 0;
}

