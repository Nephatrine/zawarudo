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
	opt.add( "equirect", 0, 1, 0, "[STRING] Map -> Projection\n  "
	         "aitoff          - Aitoff\n  "
	         "behrmann        - Behrmann Equal-Area\n  "
	         "braun           - Braun Stereographic\n  "
	         "cea             - Cylindrical Equal-Area (SP X)\n  "
	         "equirect        - Equirectangular (SP X)\n  "
	         "gall            - Gall Stereographic\n  "
	         "gall-peters     - Gall Orthographic\n  "
	         "hammer          - Hammer\n  "
	         "hobo-dyer       - Hobo-Dyer Equal-Area\n  "
	         "lambert         - Lambert Equal-Area\n  "
	         "kavrayskiy      - Kavrayskiy VII\n  "
	         "mercator        - Mercator\n  "
	         "miller          - Miller Cylindrical\n  "
	         "ortelius        - Ortelius Oval\n  "
	         "orthographic    - Orthographic\n  "
	         "plate-carree    - Plate Carree\n  "
	         "sinusoidal      - Sinusoidal\n  "
	         "wagner          - Wagner VI\n  "
	         "winkel          - Winkel III", "-m", "--map" );
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
	
	real_t parallel = ( mapType == "winkel" ) ? RAD2DEG( std::acos(
	                      2 / M_PI ) ) : 0;
	real_t meridian = 0;
	
	if ( opt.isSet( "--parallel" ) )
		opt.get( "--parallel" )->getFloat( parallel );
		
	if ( opt.isSet( "--meridian" ) )
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
	// Cylindrical Projections
	//
	
	if ( mapType == "behrmann" )
	{
		mapType = "cea";
		parallel = 30.0;
	}
	else if ( mapType == "gall-peters" )
	{
		mapType = "cea";
		parallel = 45.0;
	}
	else if ( mapType == "hobo-dyer" )
	{
		mapType = "cea";
		parallel = 37.5;
	}
	else if ( mapType == "lambert" )
	{
		mapType = "cea";
		parallel = 0;
	}
	
	//
	// Equirectangular Projections
	//
	
	if ( mapType == "plate-carree" )
	{
		mapType = "equirect";
		parallel = 0;
	}
	
	//
	// Create Map Projections
	//
	
	std::string mapTag = mapType;
	int mapNumB = parallel * 60.0;
	int mapNumA = mapNumB / 60;
	int mapNumC = meridian;
	mapNumB -= mapNumA * 60;
	
	int hemisphere = 180;
	
	using proj_ptr = std::unique_ptr<projection::base>;
	proj_ptr view, undistorted;
	
	if ( mapType == "aitoff" )
	{
		view = proj_ptr( new projection::aitoff() );
		mapNumA = mapNumB = 0;
	}
	else if ( mapType == "braun" )
	{
		view = proj_ptr( new projection::stereographic( 1 ) );
		mapNumA = mapNumB = 0;
	}
	else if ( mapType == "cea" )
		view = proj_ptr( new projection::equalarea( DEG2RAD( parallel ) ) );
	else if ( mapType == "gall" )
	{
		view = proj_ptr( new projection::stereographic( 2 ) );
		mapNumA = mapNumB = 0;
	}
	else if ( mapType == "hammer" )
	{
		view = proj_ptr( new projection::hammer() );
		mapNumA = mapNumB = 0;
	}
	else if ( mapType == "kavrayskiy" )
	{
		view = proj_ptr( new projection::kavrayskiy() );
		mapNumA = mapNumB = 0;
	}
	else if ( mapType == "mercator" )
	{
		view = proj_ptr( new projection::mercator() );
		mapNumA = mapNumB = 0;
	}
	else if ( mapType == "miller" )
	{
		view = proj_ptr( new projection::miller() );
		mapNumA = mapNumB = 0;
	}
	else if ( mapType == "ortelius" )
	{
		view = proj_ptr( new projection::ortelius() );
		mapNumA = mapNumB = 0;
	}
	else if ( mapType == "orthographic" )
	{
		view = proj_ptr( new projection::orthographic( DEG2RAD( parallel ) ) );
		undistorted = proj_ptr( new projection::orthographic( 0 ) );
		hemisphere = 90;
	}
	else if ( mapType == "sinusoidal" )
	{
		view = proj_ptr( new projection::sinusoidal() );
		mapNumA = mapNumB = 0;
	}
	else if ( mapType == "wagner" )
	{
		view = proj_ptr( new projection::wagner() );
		mapNumA = mapNumB = 0;
	}
	else if ( mapType == "winkel" )
		view = proj_ptr( new projection::winkel( DEG2RAD( parallel ) ) );
	else
	{
		view = proj_ptr( new projection::equirectangular( DEG2RAD( parallel ) ) );
		mapTag = "equirect";
	}
	
	view->meridian( DEG2RAD( meridian ) );
	
	//
	// Draw Region Map
	//
	
	plotter::gs mapRegion( view->aspect(), 768, 512 );
	mapRegion.inputRange( 0, std::min<real_t>( cells, REGION_LIMIT - 1.0 ) );
	
	for ( cell_size_t c = 0; c < cells; ++c )
		if ( view->valid( coord( geodesic[c].v ) ) )
			mapRegion.plot( view->convert( geodesic[c].v ), geodesic[c].region );
			
	//
	// Create Map Border
	//
	
	if ( undistorted )
	{
		for ( int spin = -900; spin < 900; ++spin )
		{
			mapRegion.bold( undistorted->convert( coord( DEG2RAD( -hemisphere ),
			                                      DEG2RAD( spin / 10.0 ) ) ), 1 );
			mapRegion.bold( undistorted->convert( coord( DEG2RAD( hemisphere ),
			                                      DEG2RAD( spin / 10.0 ) ) ), 1 );
		}
	}
	else
	{
		for ( int spin = -900; spin < 900; ++spin )
		{
			mapRegion.bold( view->convert( coord( DEG2RAD( -hemisphere + meridian ),
			                                      DEG2RAD( spin / 10.0 ) ) ), 1 );
			mapRegion.bold( view->convert( coord( DEG2RAD( hemisphere + meridian ),
			                                      DEG2RAD( spin / 10.0 ) ) ), 1 );
		}
	}
	
	//
	// Fill Map Gaps
	//
	
	mapRegion.fill();
	
	//
	// Draw Graticules
	//
	
	for ( int graticule = -90; graticule < 90; graticule += 15 )
		for ( int spin = -540; spin < 540; ++spin )
		{
			coord c( DEG2RAD( spin / 3.0 + meridian ), DEG2RAD( graticule ) );
			
			if ( view->valid( c ) ) mapRegion.set( view->convert( c ) , 1 );
		}
		
	for ( int graticule = -180; graticule < 180; graticule += 15 )
		for ( int spin = -540; spin < 540; ++spin )
		{
			coord c( DEG2RAD( graticule + meridian ), DEG2RAD( spin / 6.0 ) );
			
			if ( view->valid( c ) ) mapRegion.set( view->convert( c ) , 1 );
		}
		
	//
	// Write Map
	//
	
	{
		std::stringstream fileMap;
		fileMap << nameOut << "_" << iterations << "_region." << mapTag;
		
		if ( mapNumA > 0 || mapNumB > 0 ) fileMap << "-p" << mapNumA;
		
		if ( mapNumB > 0 ) fileMap << "-" << mapNumB;
		
		if ( mapNumC > 0 ) fileMap << "-m" << mapNumC;
		
		fileMap << ".png";
		std::cout << "saving map " << fileMap.str() << std::endl;
		mapRegion.write( fileMap.str() );
	}
	
	return 0;
}

