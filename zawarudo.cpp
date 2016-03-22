// ZaWarudo Headers
#include "geodesic.hpp"
#include "projection.hpp"

// Third-Party Headers
#include "lib/ezOptionParser.hpp"

static void show_usage( ez::ezOptionParser &opt )
{
	std::string usage;
	opt.getUsage( usage, 80, ez::ezOptionParser::INTERLEAVE );
	std::cout << usage;
}

static std::string getMapFile( const std::string &world,
                               const std::string &dataset, const std::string &projection,
                               const int iterations , const zw::real_t parallel, const zw::real_t meridian )
{
	int minutes, degrees;
	std::stringstream mapFile;
	mapFile << world << "_" << iterations << "_" << dataset << "." << projection;
	
	minutes = parallel * 60;
	degrees = minutes / 60;
	minutes -= degrees * 60;
	
	if ( degrees > 0 || minutes > 0 ) mapFile << "-p" << degrees;
	
	if ( minutes > 0 ) mapFile << "-" << minutes;
	
	minutes = meridian * 60;
	degrees = minutes / 60;
	minutes -= degrees * 60;
	
	if ( degrees > 0 || minutes > 0 ) mapFile << "-m" << degrees;
	
	if ( minutes > 0 ) mapFile << "-" << minutes;
	
	mapFile << ".png";
	return mapFile.str();
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
	opt.add( "",  1, 1, 0, "[#] Icosahedron Subdivisions", "-i", "--subdivide" );
	opt.add( "",  0, 1, 0, "[#] Terrain Perturbations", "-p", "--perturb" );
	opt.add( "",  0, 1, 0, "[STRING] Slug Of Alternate World To Load", "--base" );
	opt.add( "world", 0, 1, 0, "[STRING] World Filename Slug", "-w", "--world" );
	opt.add( "", 0, 1, 0, "[KM] Radius of Sphere", "-R", "--radius" );
	opt.add( "", 0, 1, 0, "[%] Ocean Coverage", "-H", "--hydro" );
	
	// Mapping Options
	opt.add( "", 0, 1, 0, "[STRING] Map -> Projection\n  "
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
	{
		opt.get( "-i" )->getInt( iterations );
		assert( iterations >= 0 && iterations <= SUBDIVIDE_LIMIT );
	}
	
	int perturbations = 0;
	
	if ( opt.isSet( "-p" ) )
	{
		opt.get( "-p" )->getInt( perturbations );
		assert( perturbations > 0 );
	}
	
	std::string nameIn, nameOut;
	opt.get( "-w" )->getString( nameOut );
	
	if ( opt.isSet( "--base" ) )
		opt.get( "--base" )->getString( nameIn );
	else
		nameIn = nameOut;
		
	std::string mapType = "equirect";
	bool genMap = false;
	
	if ( opt.isSet( "-m" ) )
	{
		opt.get( "-m" )->getString( mapType );
		genMap = true;
	}
	
	real_t parallel = ( mapType == "winkel" ) ? RAD2DEG( std::acos(
	                      2 / M_PI ) ) : 0;
	                      
	if ( opt.isSet( "--parallel" ) )
	{
		opt.get( "--parallel" )->getFloat( parallel );
		assert( parallel >= 0 && parallel < 90 );
	}
	
	real_t meridian = 0;
	
	if ( opt.isSet( "--meridian" ) )
	{
		opt.get( "--meridian" )->getFloat( meridian );
		assert( meridian >= -180.0 && meridian <= 180.0 );
	}
	
	real_t radius = 0;
	
	if ( opt.isSet( "-R" ) )
	{
		opt.get( "-R" )->getFloat( radius );
		assert( radius > 0 );
	}
	
	real_t hydro = 0;
	
	if ( opt.isSet( "-H" ) )
	{
		opt.get( "-H" )->getFloat( hydro );
		hydro *= 0.01;
		assert( hydro >= 0 && hydro <= 1.0 );
	}
	
	//
	// Seed Generator
	//
	
	std::random_device seedGen;
	auto seed = seedGen();
	std::cout << "using seed " << seed << std::endl;
	std::mt19937_64 rng( seedGen() );
	
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
	bool save = ( nameIn != nameOut );
	
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
		save = true;
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
	// Randomize Terrain
	//
	
	if ( perturbations > 0 )
	{
		int tenper = perturbations / 10;
		int ci = 0, cp = 0;
		
		std::cout << "running " << perturbations << " perturbations" << std::endl;
		
		for ( int i = 0; i < perturbations; ++i, ++ci )
		{
			if ( ci == tenper )
			{
				cp += 10;
				ci = 0;
				std::cout << "  " << cp << "%" << std::endl;
			}
			
			geoData::perturb( geodesic, cells, rng );
		}
		
		save = true;
	}
	
	//
	// Elevations
	//
	
	std::cout << "calculating elevations" << std::endl;
	
	real_t seaLevel = geoData::findElevation( geodesic, cells, hydro );
	
	if ( radius > 0 )
	{
		for ( cell_size_t c = 0; c < cells; ++c )
		{
			real_t multiplier = ( geodesic[c].v.magnitude() / seaLevel ) * radius;
			geodesic[c].v = geodesic[c].v.normalize() * multiplier;
		}
		
		save = true;
	}
	
	range_t extremes = geoData::extremes( geodesic, cells );
	seaLevel = geoData::findElevation( geodesic, cells, hydro, extremes );
	
	if ( hydro > 0 || radius > 0 )
	{
		extremes = geoData::rescale( geodesic, cells, seaLevel, hydro, extremes );
		save = true;
	}
	
	std::cout << "  high point: " << extremes.second << " km" << std::endl;
	
	if ( hydro > 0 && seaLevel > extremes.first )
		std::cout << "  sea level:  " << seaLevel << " km" << std::endl;
		
	std::cout << "  low point:  " << extremes.first << " km" << std::endl;
	
	//
	// Output Geodesic
	//
	
	if ( save == true )
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
	
	using proj_ptr = std::unique_ptr<projection::base>;
	proj_ptr view;
	
	if ( mapType == "aitoff" )
	{
		view = proj_ptr( new projection::aitoff() );
		parallel = 0;
	}
	else if ( mapType == "braun" )
	{
		view = proj_ptr( new projection::stereographic( 1 ) );
		parallel = 0;
	}
	else if ( mapType == "cea" )
		view = proj_ptr( new projection::equalarea( DEG2RAD( parallel ) ) );
	else if ( mapType == "gall" )
	{
		view = proj_ptr( new projection::stereographic( 2 ) );
		parallel = 0;
	}
	else if ( mapType == "hammer" )
	{
		view = proj_ptr( new projection::hammer() );
		parallel = 0;
	}
	else if ( mapType == "kavrayskiy" )
	{
		view = proj_ptr( new projection::kavrayskiy() );
		parallel = 0;
	}
	else if ( mapType == "mercator" )
	{
		view = proj_ptr( new projection::mercator() );
		parallel = 0;
	}
	else if ( mapType == "miller" )
	{
		view = proj_ptr( new projection::miller() );
		parallel = 0;
	}
	else if ( mapType == "ortelius" )
	{
		view = proj_ptr( new projection::ortelius() );
		parallel = 0;
	}
	else if ( mapType == "orthographic" )
		view = proj_ptr( new projection::orthographic( DEG2RAD( parallel ) ) );
	else if ( mapType == "sinusoidal" )
	{
		view = proj_ptr( new projection::sinusoidal() );
		parallel = 0;
	}
	else if ( mapType == "wagner" )
	{
		view = proj_ptr( new projection::wagner() );
		parallel = 0;
	}
	else if ( mapType == "winkel" )
		view = proj_ptr( new projection::winkel( DEG2RAD( parallel ) ) );
	else
	{
		view = proj_ptr( new projection::equirectangular( DEG2RAD( parallel ) ) );
		mapType = "equirect";
	}
	
	view->meridian( DEG2RAD( meridian ) );
	
	//
	// Draw Maps
	//
	
	plotter::gs map( view->aspect(), 768, 512 );
	
	if ( genMap )
	{
		std::string name = getMapFile( nameOut, "region", mapType, iterations, parallel,
		                               meridian );
		std::cout << "saving map " << name << std::endl;
		map.clear();
		map.inputRange( 0, std::min<real_t>( cells, REGION_LIMIT - 1.0 ) );
		
		for ( cell_size_t c = 0; c < cells; ++c )
			if ( view->valid( coord( geodesic[c].v ) ) )
				map.plot( view->convert( geodesic[c].v ), geodesic[c].region );
				
		view->drawBorder( map );
		map.fill();
		view->drawGraticule( map );
		map.write( name );
	}
	
	if ( genMap  && extremes.first < extremes.second )
	{
	
		std::string name = getMapFile( nameOut, "height", mapType, iterations, parallel,
		                               meridian );
		std::cout << "saving map " << name << std::endl;
		map.clear();
		map.inputRange( extremes );
		
		for ( cell_size_t c = 0; c < cells; ++c )
			if ( view->valid( coord( geodesic[c].v ) ) )
				map.plot( view->convert( geodesic[c].v ), geodesic[c].v.magnitude() );
				
		view->drawBorder( map );
		map.fill();
		view->drawGraticule( map );
		map.write( name );
	}
	
	if ( genMap  && hydro > 0 )
	{
		std::string name = getMapFile( nameOut, "land", mapType, iterations, parallel,
		                               meridian );
		std::cout << "saving map " << name << std::endl;
		map.clear();
		map.inputRange( extremes );
		
		for ( cell_size_t c = 0; c < cells; ++c )
			if ( view->valid( coord( geodesic[c].v ) ) )
			{
				real_t magnitude = geodesic[c].v.magnitude();
				map.plot( view->convert( geodesic[c].v ),
				          magnitude < seaLevel ? extremes.first : magnitude );
			}
			
		view->drawBorder( map );
		map.fill();
		view->drawGraticule( map );
		map.write( name );
	}
	
	return 0;
}

