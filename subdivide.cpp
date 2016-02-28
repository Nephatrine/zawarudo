#include <iostream>
#include <sstream>
#include <vector>

#include "basedata.hpp"
#include "monomap.hpp"
#include "subdivide.hpp"

#include "lib/ezOptionParser.hpp"

static void Usage( ez::ezOptionParser &opt )
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
			geodesic[i].v /= geodesic[i].v.magnitude();
			geodesic[i].region = i;
		}
		
		iterationsCurrent = 0;
		cellsCurrent = 12;
	}
	
	//
	// Subdivide
	//
	
	bool greaterNode = true;
	cell_size_t regionScore[12] = {0};
	
	while ( iterationsCurrent < iterationsNeeded )
	{
		// Save Previous Iteration
		saveBaseData( iterationsCurrent, geodesic, cellsCurrent );
		
		std::cout << "Starting Iteration " << ++iterationsCurrent << std::endl;
		
		std::vector<deferral> deferList;
		
		cell_size_t extant = cellsCurrent, created, other;
		int regionA, regionB;
		
		for ( cell_size_t parent = 0; parent < extant; ++parent )
		{
			for ( int spoke = 0; spoke < 6; ++spoke )
			{
				cell_size_t child = geodesic[parent].link[spoke];
				
				if ( child >= extant )
					continue; // Don't process already split spokes.
					
				// Create New Spoke
				
				created = cellsCurrent++;
				geodesic[created].v = ( ( geodesic[parent].v + geodesic[child].v ) /
				                        2 ).normalize();
				                        
				// Assign Region
				
				regionA = geodesic[parent].region;
				regionB = geodesic[child].region;
				
				if ( regionScore[regionA] > regionScore[regionB] )
					geodesic[created].region = regionB;
				else if ( regionScore[regionB] > regionScore[regionA] )
					geodesic[created].region = regionA;
				else if ( greaterNode = !greaterNode )
					geodesic[created].region = std::max( regionA, regionB );
				else
					geodesic[created].region = std::min( regionB, regionA );
					
				// Region Balancing?
				
				++regionScore[geodesic[created].region];
				
				//
				// Here's how the linking works. In the original hexagon, there
				// is a "center" which is the Parent node. This center has 6
				// spokes leading to the vertices of the hexagon. We are
				// creating a new point between the Parent and one of the spokes
				// we'll call the Child. The Created node needs to be linked to
				// both the Parent and Child. In the original hexagon, we also
				// need to know the Child's sibling nodes on either side from
				// the parent: CounterClockwise and Clockwise.
				//
				// During this iteration, the links between these two nodes and
				// both the Parent and Child should be subdivided just like this
				// Parent<->Child spoke, creating four new nodes. Our newly
				// Created node needs to link to all four of these. Some of them
				// are likely to have already been created previously in the
				// subdivision process and we can link straight to them. Others
				// will not have been created yet and we'll need to set up
				// deferred links which will be created when the nodes are.
				//
				// link[0] = Parent
				// link[1] = Parent<->CounterClockwise
				// link[2] = CounterClockwise<->Child
				// link[3] = Child
				// link[4] = Clockwise<->Child
				// link[5] = Parent<->Clockwise
				//
				
				// Create Link 0
				
				geodesic[created].link[0] = parent;
				geodesic[parent].link[spoke] = created;
				
				// Create Link 1
				
				other = geodesic[parent].counterClockwise( spoke );
				
				if ( other >= extant )
				{
					geodesic[created].link[1] = other;
					
					if ( geodesic[other].link[0] == parent )
						other = geodesic[other].link[3];
					else
						other = geodesic[other].link[0];
				}
				else
					deferList.push_back( deferral( parent, other, 1, created ) );
					
				// Create Link 2
				
				for ( int s = 0; s < 6; ++s )
				{
					if ( geodesic[other].link[s] == child )
					{
						deferList.push_back( deferral( child, other, 2, created ) );
						break;
					}
					else if ( ( geodesic[geodesic[other].link[s]].link[0] == child
					            && geodesic[geodesic[other].link[s]].link[3] == other )
					          || ( geodesic[geodesic[other].link[s]].link[0] == other
					               && geodesic[geodesic[other].link[s]].link[3] == child ) )
					{
						geodesic[created].link[2] = geodesic[other].link[s];
						break;
					}
				}
				
				// Create Link 3
				
				geodesic[created].link[3] = child;
				
				for ( int s = 0; s < 6; ++s )
					if ( geodesic[child].link[s] == parent )
					{
						geodesic[child].link[s] = created;
						break;
					}
					
				// Create Link 5
				
				other = geodesic[parent].clockwise( spoke );
				
				if ( other >= extant )
				{
					geodesic[created].link[5] = other;
					
					if ( geodesic[other].link[0] == parent )
						other = geodesic[other].link[3];
					else
						other = geodesic[other].link[0];
				}
				else
					deferList.push_back( deferral( parent, other, 5, created ) );
					
				// Create Link 4
				
				for ( int s = 0; s < 6; ++s )
				{
					if ( geodesic[other].link[s] == child )
					{
						deferList.push_back( deferral( child, other, 4, created ) );
						break;
					}
					else if ( ( geodesic[geodesic[other].link[s]].link[0] == child
					            && geodesic[geodesic[other].link[s]].link[3] == other )
					          || ( geodesic[geodesic[other].link[s]].link[0] == other
					               && geodesic[geodesic[other].link[s]].link[3] == child ) )
					{
						geodesic[created].link[4] = geodesic[other].link[s];
						break;
					}
				}
			}
		}
		
		// Process Deferred Links
		
		for ( auto const &d : deferList )
		{
			for ( int s = 0; s < 6; ++s )
			{
				other = geodesic[d.a].link[s];
				
				if ( ( geodesic[other].link[0] == d.a && geodesic[other].link[3] == d.b )
				        || ( geodesic[other].link[3] == d.a && geodesic[other].link[0] == d.b ) )
				{
					geodesic[d.c].link[d.s] = other;
					break;
				}
				
				// If we got here it means a deferral slipped through the
				// cracks!
				assert( s != 5 );
			}
		}
		
		deferList.clear();
	}
	
	assert( cellsCurrent == cellsNeeded );
	assert( iterationsCurrent == iterationsNeeded );
	saveBaseData( iterationsCurrent, geodesic, cellsCurrent );
	
	//
	// Write Regional Map
	// Right now this is one of the few ways to visualize the output so far.
	//
	
	std::cout << "Saving Regional Map" << std::endl;
	monomap<768, 512> mapRegion;
	mapRegion.max = 11;
	
	for ( int i = 0; i < cellsCurrent; ++i )
		mapRegion.setPixel( geodesic[i].v.latitude(), geodesic[i].v.longitude(),
		                    geodesic[i].region );
		                    
	mapRegion.fill();
	mapRegion.write( "regions.png" );
	
	return 0;
}

