// ZaWarudo Headers
#include "geodesic.hpp"
#include "image.hpp"
#include "map.hpp"

// Third-Party Headers
#include "lib/ezOptionParser.hpp"

static void show_usage( ez::ezOptionParser &opt )
{
	std::string usage;
	opt.getUsage( usage, 80, ez::ezOptionParser::INTERLEAVE );
	std::cout << usage;
}

int main( int argc, const char *argv[] )
{
	using namespace zw;
	
	//
	// Commandline Arguments
	//
	
	ez::ezOptionParser opt;
	opt.overview = "\nZA WARUDO\nCreate worlds from subdivided icosahedrons.";
	opt.syntax = "zawarudo [OPTIONS] -i [ITERATIONS]";
	opt.footer =
	    "\nCopyright (C) 2016 Daniel Wolf\nThis program is free and without warranty.\n\n";
	opt.add( "",      0, 0, 0, "Display usage instructions and exit.", "-h",
	         "--help" );
	opt.add( "",      0, 0, 0, "Regenerate geodesic data from scratch.", "-f",
	         "--force" );
	opt.add( "",      1, 1, 0, "Number of times to subdivide icosahedron.", "-i",
	         "--subdivide" );
	opt.add( "",      0, 1, 0, "Filename slug for loaded files.", "--base" );
	opt.add( "world", 0, 1, 0, "Filename slug for generated files.", "-o",
	         "--out" );
	opt.add( "equirect", 0, 1, 0, "Map type to generate. [equirect]", "-m",
	         "--map" );
	opt.add( "",         0, 1, 0,
	         "Standard parallel in degrees (if applies to projection).", "--parallel" );
	opt.parse( argc, argv );
	
	if ( opt.isSet( "-h" ) )
	{
		show_usage( opt );
		return 0;
	}
	
	std::vector<std::string> badOptions;
	
	if ( !opt.gotRequired( badOptions ) )
	{
		for ( size_t i = 0; i < badOptions.size(); ++i )
			std::cerr << "Missing Required Option: " << badOptions[i] << std::endl;
			
		show_usage( opt );
		return 1;
	}
	
	if ( !opt.gotExpected( badOptions ) )
	{
		for ( size_t i = 0; i < badOptions.size(); ++i )
			std::cerr << "Unexpected Number of Arguments: " << badOptions[i] << std::endl;
			
		show_usage( opt );
		return 1;
	}
	
	//
	// User Options
	//
	
	bool forceRegen = false;
	
	if ( opt.isSet( "-f" ) )
		forceRegen = true;
		
	int iterations = -1;
	
	if ( opt.isSet( "-i" ) )
		opt.get( "-i" )->getInt( iterations );
		
	if ( iterations < 0 )
	{
		std::cerr << "Cannot produce negative subdivisions." << std::endl;
		return 1;
	}
	
	if ( iterations > SUBDIVIDE_LIMIT )
	{
		std::cerr << "Cannot produce >" << SUBDIVIDE_LIMIT << " subdivisions." <<
		          std::endl;
		return 1;
	}
	
	std::string nameIn, nameOut;
	opt.get( "-o" )->getString( nameOut );
	
	if ( opt.isSet( "--base" ) )
		opt.get( "--base" )->getString( nameIn );
	else
		nameIn = nameOut;
		
	std::string mapType;
	opt.get( "-m" )->getString( mapType );
	
	float parallel = 0;
	
	if ( opt.isSet( "--parallel" ) )
		opt.get( "--parallel" )->getFloat( parallel );
		
	if ( parallel < 0 || parallel >= 90 )
	{
		std::cerr << "Standard parallel must be within range 0 <= SP < 90." <<
		          std::endl;
		return 1;
	}
	
	//
	// Allocate Memory
	//
	
	auto cells = cellsPerIteration( iterations );
	std::unique_ptr<geoData[]> geodesic;
	
	try
	{
		geodesic = std::unique_ptr<geoData[]>( new geoData[cells] );
	}
	catch ( std::bad_alloc &err )
	{
		std::cerr << "Failed to allocate " << ( sizeof( geoData )*cells ) <<
		          " bytes for geodesic.\nTry a smaller subdivision count.\n" << std::endl;
		throw;
	}
	
	//
	// Load Base Data
	//
	
	cell_size_t generated = 0;
	int pass = -1;
	
	if ( !forceRegen )
	{
		std::stringstream fileIn;
		fileIn << nameIn << "_" << iterations << ".dat";
		
		if ( geoData::load( geodesic, cells, fileIn.str() ) )
		{
			std::cout << "loaded geodesic " << fileIn.str() << std::endl;
			pass = iterations;
			generated = cells;
		}
	}
	
	if ( pass == -1 )
	{
		geoData::icosahedron( geodesic, generated );
		std::cout << "loaded icosahedron" << std::endl;
		++pass;
	}
	
	//
	// Create Geodesic If Needed
	//
	
	while ( pass < iterations )
	{
		std::cout << "running subdivision pass " << ++pass << std::endl;
		geoData::subdivide( geodesic, generated );
	}
	
	assert( pass == iterations );
	assert( cells == generated );
	
	//
	// Output Geodesic
	//
	
	{
		std::stringstream fileOut;
		fileOut << nameOut << "_" << iterations << ".dat";
		std::cout << "saving geodesic " << fileOut.str() << std::endl;
		geoData::save( geodesic, cells, fileOut.str() );
	}
	
	//
	// Write Regional Map
	// Right now this is one of the few ways to visualize the output so far.
	//
	
	//
	// Create Maps
	//
	
	std::unique_ptr<map::project::base> projection;
	
	if ( mapType == "geographic" || mapType == "plate-carree" )
		projection = std::unique_ptr<map::project::base>( new
		             map::project::equirectangular( 0 ) );
	else
		projection = std::unique_ptr<map::project::base>( new
		             map::project::equirectangular( DEG2RAD( parallel ) ) );
		             
	image::greyscale regionalMap( projection->aspect(), 768, 512 );
	regionalMap.range( 0, std::min<float>( cells, REGION_LIMIT - 1.0 ) );
	
	for ( cell_size_t c = 0; c < cells; ++c )
		regionalMap.plot( projection->point( map::geoCoord( geodesic[c].v ) ),
		                  geodesic[c].region );
		                  
	{
		std::stringstream fileMap;
		fileMap << nameOut << "_" << iterations << "_region.png";
		std::cout << "saving map " << fileMap.str() << std::endl;
		regionalMap.fill();
		regionalMap.write( fileMap.str() );
	}
	
	return 0;
}
