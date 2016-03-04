// ZaWarudo Headers
#include "geodesic.hpp"
#include "plotter.hpp"
#include "projection.hpp"

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
	opt.add( "", 0, 0, 0, "Display usage instructions and exit.", "-h",
	         "--help" );
	opt.add( "", 0, 0, 0, "Regenerate geodesic data from scratch.", "-f",
	         "--force" );
	         
	// Geodesic Options
	opt.add( "", 1, 1, 0, "[#] Icosahedron Subdivisions", "-i", "--subdivide" );
	opt.add( "", 0, 1, 0, "[STRING] Slug Of Alternate World To Load", "--base" );
	opt.add( "world", 0, 1, 0, "[STRING] World Filename Slug", "-w", "--world" );
	
	// Mapping Options
	opt.add( "equirectangular", 0, 1, 0, "[STRING] Map -> Projection\n  "
	         "braun-stereo    - Braun Stereographic\n  "
	         "equirectangular - Equirectangular (SP X)\n  "
	         "gall-stereo     - Gall Stereographic\n  "
	         "mercator        - Mercator\n  "
	         "miller-cyl      - Miller Cylindrical\n  "
	         "plate-carree    - Plate Caree", "-m", "--map" );
	opt.add( "0.0", 0, 1, 0, "[DEGREES] Map -> Standard Parallel", "--parallel" );
	opt.add( "0.0", 0, 1, 0, "[DEGREES] Map -> Prime Meridian", "--meridian" );
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
	opt.get( "-w" )->getString( nameOut );
	
	if ( opt.isSet( "--base" ) )
		opt.get( "--base" )->getString( nameIn );
	else
		nameIn = nameOut;
		
	std::string mapType;
	opt.get( "-m" )->getString( mapType );
	
	real_t parallel = 0;
	real_t meridian = 0;
	
	opt.get( "--parallel" )->getFloat( parallel );
	opt.get( "--meridian" )->getFloat( meridian );
	
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
	bool sav = false;
	
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
		sav = true;
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
	
	if ( sav == true )
	{
		std::stringstream fileOut;
		fileOut << nameOut << "_" << iterations << ".dat";
		std::cout << "saving geodesic " << fileOut.str() << std::endl;
		geoData::save( geodesic, cells, fileOut.str() );
	}
	
	//
	// Create Maps
	//
	
	std::unique_ptr<projection::base> mapView;
	
	if ( mapType == "braun-stereo" )
		mapView = std::unique_ptr<projection::base>( new projection::stereographic(
		              1 ) );
	else if ( mapType == "gall-stereo" )
		mapView = std::unique_ptr<projection::base>( new projection::stereographic(
		              2 ) );
	else if ( mapType == "mercator" )
		mapView = std::unique_ptr<projection::base>( new projection::mercator() );
	else if ( mapType == "miller-cyl" )
		mapView = std::unique_ptr<projection::base>( new projection::miller() );
	else if ( mapType == "plate-carree" )
		mapView = std::unique_ptr<projection::base>( new projection::equirectangular(
		              0 ) );
	else
		mapView = std::unique_ptr<projection::base>( new projection::equirectangular(
		              DEG2RAD( parallel ) ) );
		              
	mapView->meridian( DEG2RAD( meridian ) );
	
	plotter::gs mapRegion( mapView->aspect(), 768, 512 );
	mapRegion.inputRange( 0, std::min<real_t>( cells, REGION_LIMIT - 1.0 ) );
	
	for ( cell_size_t c = 0; c < cells; ++c )
		mapRegion.plot( mapView->convert( geodesic[c].v ), geodesic[c].region );
		
	{
		std::stringstream fileMap;
		fileMap << nameOut << "_" << iterations << "_region.png";
		std::cout << "saving map " << fileMap.str() << std::endl;
		mapRegion.fill();
		mapRegion.write( fileMap.str() );
	}
	
	return 0;
}

