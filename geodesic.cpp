// ZaWarudo Headers
#include "geodesic.hpp"

// Utility Headers
#include "serialize.hpp"

#if REGION_LIMIT < 12
#	error "REGION_LIMIT <12 is not supported."
#endif

//
// Internal Stuff
//

struct deferral
{
	deferral() = default;
	deferral( const deferral & ) = default;
	deferral &operator=( const deferral & ) = default;
	
	deferral( zw::cell_size_t A, zw::cell_size_t B, int spoke,
	          zw::cell_size_t targ )
		: a( A ), b( B ), s( spoke ), t( targ )
	{}
	
	zw::cell_size_t a, b, t;
	int s;
};

static zw::cell_size_t region_score[REGION_LIMIT] = {0};

static zw::region_t region_split( const zw::region_t a, const zw::region_t b )
{
	static bool flip = false;
	
	if ( region_score[a] > region_score[b] )
		return b;
	else if ( region_score[a] < region_score[b] )
		return a;
		
	return ( flip = !flip ) ? std::max( a, b ) : std::min( a, b );
}

//
// Public API
//

void zw::geoData::subdivide( geo_ptr &data, cell_size_t &extant )
{
	auto created = extant;
	
	// Store Links To Future-Nodes
	std::vector<deferral> deferred;
	
	for ( cell_size_t parent = 0; parent < extant; ++parent )
	{
		for ( int spoke = 0; spoke < 6; ++spoke )
		{
			auto child = data[parent].link[spoke];
			
			if ( child >= extant )
				continue; // we already split this pair
				
			// Create new node at the modpoint of pair
			
			for ( int s = 0; s < 6; ++s )
				data[created].link[s] = nolink - 1;
				
			data[created].v = ( data[parent].v + data[child].v ) / 2;
			data[created].v.normalize();
			
			data[created].region = ( created < REGION_LIMIT ) ? created : region_split(
			                           data[parent].region, data[child].region );
			region_score[data[created].region] += 1;
			
			//
			// Here's how the linking works. In the original hexagon, there is
			// a "center" which is the parent node. This center has 6 spokes
			// leading to the vertices of the hexagon. We are creating a new
			// point between the parent and one of the spokes -- the child. The
			// created node needs to be linked to both the parent and child. In
			// the original hexagon, we also need to know the child's sibling
			// nodes on either side from the parent - counter-clockwise and
			// clockwise.
			//
			// During this iteration, the links between these two nodes and both
			// the parent and child should be subdivided just like this spoke,
			// creating four new nodes. Our newly created node needs to link to
			// all four of these. Some of them are likely to have already been
			// created previously in the subdivision process and we can link
			// straight to them. Others will not have been created yet and we'll
			// need to set up deferred links which will be created when the
			// nodes are.
			//
			// link[0] = parent
			// link[1] = new node between parent and counter-clockwise sibling
			// link[2] = new node between counter-clockwise sibling and child
			// link[3] = child
			// link[4] = new node between clockwise sibling and child
			// link[5] = new node between parent and clockwise sibling
			//
			
			// Link To Parent
			
			data[created].link[0] = parent;
			data[parent].link[spoke] = created;
			
			// Link To Counter-Clockwise Siblings
			
			auto neighbor = data[parent].prevNeighbor( spoke );
			
			if ( neighbor >= extant )
			{
				data[created].link[1] = neighbor;
				
				if ( data[neighbor].link[0] == parent )
					neighbor = data[neighbor].link[3];
				else
					neighbor = data[neighbor].link[0];
			}
			else
				deferred.push_back( deferral( parent, neighbor, 1, created ) );
				
			for ( int s = 0; s < 6; ++s )
			{
				if ( data[neighbor].link[s] == child )
				{
					deferred.push_back( deferral( child, neighbor, 2, created ) );
					break;
				}
				else if ( ( data[data[neighbor].link[s]].link[0] == child
				            && data[data[neighbor].link[s]].link[3] == neighbor )
				          || ( data[data[neighbor].link[s]].link[0] == neighbor
				               && data[data[neighbor].link[s]].link[3] == child ) )
				{
					data[created].link[2] = data[neighbor].link[s];
					break;
				}
			}
			
			// Link To Child
			
			data[created].link[3] = child;
			
			for ( int s = 0; s < 6; ++s )
			{
				if ( data[child].link[s] == parent )
				{
					data[child].link[s] = created;
					break;
				}
			}
			
			// Link To Clockwise Siblings
			
			neighbor = data[parent].nextNeighbor( spoke );
			
			if ( neighbor >= extant )
			{
				data[created].link[5] = neighbor;
				
				if ( data[neighbor].link[0] == parent )
					neighbor = data[neighbor].link[3];
				else
					neighbor = data[neighbor].link[0];
			}
			else
				deferred.push_back( deferral( parent, neighbor, 5, created ) );
				
			for ( int s = 0; s < 6; ++s )
			{
				if ( data[neighbor].link[s] == child )
				{
					deferred.push_back( deferral( child, neighbor, 4, created ) );
					break;
				}
				else if ( ( data[data[neighbor].link[s]].link[0] == child
				            && data[data[neighbor].link[s]].link[3] == neighbor )
				          || ( data[data[neighbor].link[s]].link[0] == neighbor
				               && data[data[neighbor].link[s]].link[3] == child ) )
				{
					data[created].link[4] = data[neighbor].link[s];
					break;
				}
			}
			
			// We have finished creating this node.
			
			++created;
		}
	}
	
	for ( auto const &d : deferred )
	{
		for ( int s = 0; s < 6; ++s )
		{
			auto other = data[d.a].link[s];
			
			if ( ( data[other].link[0] == d.a && data[other].link[3] == d.b )
			        || ( data[other].link[3] == d.a && data[other].link[0] == d.b ) )
			{
				data[d.t].link[d.s] = other;
				break;
			}
			
			assert( s != 5 ); // we should never get here
		}
	}
	
	// We're Done
	extant = created;
}

void zw::geoData::icosahedron( geo_ptr &data, cell_size_t &extant )
{
	real_t t = ( 1.0 + std::sqrt( 5.0 ) ) / 2.0;
	real_t d = std::sqrt( 1.0 + std::pow( t, 2.0 ) );
	real_t tau = t / d;
	real_t one = 1 / d;
	
	data[0].link[0] = 1;
	data[0].link[1] = 5;
	data[0].link[2] = 2;
	data[0].link[3] = 4;
	data[0].link[4] = 3;
	data[0].v.x = one;
	data[0].v.y = 0;
	data[0].v.z = tau;
	
	data[1].link[0] = 0;
	data[1].link[1] = 3;
	data[1].link[2] = 6;
	data[1].link[3] = 7;
	data[1].link[4] = 5;
	data[1].v.x = -one;
	data[1].v.y = 0;
	data[1].v.z = tau;
	
	data[2].link[0] = 0;
	data[2].link[1] = 5;
	data[2].link[2] = 9;
	data[2].link[3] = 8;
	data[2].link[4] = 4;
	data[2].v.x = tau;
	data[2].v.y = -one;
	data[2].v.z = 0;
	
	data[3].link[0] = 0;
	data[3].link[1] = 4;
	data[3].link[2] = 10;
	data[3].link[3] = 6;
	data[3].link[4] = 1;
	data[3].v.x = 0;
	data[3].v.y = tau;
	data[3].v.z = one;
	
	data[4].link[0] = 0;
	data[4].link[1] = 2;
	data[4].link[2] = 8;
	data[4].link[3] = 10;
	data[4].link[4] = 3;
	data[4].v.x = tau;
	data[4].v.y = one;
	data[4].v.z = 0;
	
	data[5].link[0] = 0;
	data[5].link[1] = 1;
	data[5].link[2] = 7;
	data[5].link[3] = 9;
	data[5].link[4] = 2;
	data[5].v.x = 0;
	data[5].v.y = -tau;
	data[5].v.z = one;
	
	data[6].link[0] = 11;
	data[6].link[1] = 7;
	data[6].link[2] = 1;
	data[6].link[3] = 3;
	data[6].link[4] = 10;
	data[6].v.x = -tau;
	data[6].v.y = one;
	data[6].v.z = 0;
	
	data[7].link[0] = 11;
	data[7].link[1] = 9;
	data[7].link[2] = 5;
	data[7].link[3] = 1;
	data[7].link[4] = 6;
	data[7].v.x = -tau;
	data[7].v.y = -one;
	data[7].v.z = 0;
	
	data[8].link[0] = 11;
	data[8].link[1] = 10;
	data[8].link[2] = 4;
	data[8].link[3] = 2;
	data[8].link[4] = 9;
	data[8].v.x = one;
	data[8].v.y = 0;
	data[8].v.z = -tau;
	
	data[9].link[0] = 11;
	data[9].link[1] = 8;
	data[9].link[2] = 2;
	data[9].link[3] = 5;
	data[9].link[4] = 7;
	data[9].v.x = 0;
	data[9].v.y = -tau;
	data[9].v.z = -one;
	
	data[10].link[0] = 11;
	data[10].link[1] = 6;
	data[10].link[2] = 3;
	data[10].link[3] = 4;
	data[10].link[4] = 8;
	data[10].v.x = 0;
	data[10].v.y = tau;
	data[10].v.z = -one;
	
	data[11].link[0] = 6;
	data[11].link[1] = 10;
	data[11].link[2] = 8;
	data[11].link[3] = 9;
	data[11].link[4] = 7;
	data[11].v.x = -one;
	data[11].v.y = 0;
	data[11].v.z = -tau;
	
	for ( cell_size_t c = 0; c < 12; ++c )
	{
		data[c].link[5] = geoData::nolink;
		data[c].v.normalize();
		data[c].region = c;
		region_score[c] += 1;
	}
	
	extant = 12;
}

bool zw::geoData::load( geo_ptr &data, const cell_size_t size,
                        const std::string &file )
{
	serialize::input handle( file );
	
	if ( handle.exists() )
	{
		// if we have same geoData representation and enough cells to load
		
		if ( sizeof( geoData ) == handle.read<std::size_t>() )
		{
			if ( size <= handle.read<cell_size_t>() )
			{
				for ( cell_size_t c = 0; c < size; ++c )
				{
					handle.read( data[c].link, 6 );
					handle.read( data[c].v.x );
					handle.read( data[c].v.y );
					handle.read( data[c].v.z );
					handle.read( data[c].region );
				}
				
				return true;
			}
		}
	}
	
	return false;
}

void zw::geoData::save( geo_ptr &data, const cell_size_t size,
                        const std::string &file )
{
	serialize::output handle( file );
	
	handle.write<std::size_t>( sizeof( geoData ) );
	handle.write( size );
	
	for ( cell_size_t c = 0; c < size; ++c )
	{
		handle.write( data[c].link, 6 );
		handle.write( data[c].v.x );
		handle.write( data[c].v.y );
		handle.write( data[c].v.z );
		handle.write( data[c].region );
	}
}

