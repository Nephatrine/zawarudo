#ifndef SUBDIVIDE_HPP
#define SUBDIVIDE_HPP

#include "config.hpp"

namespace zw
{
	struct deferral
	{
		deferral() = default;
		deferral( const deferral & ) = default;
		deferral &operator=( const deferral & ) = default;
		
		deferral( cell_size_t nodeA, cell_size_t nodeB, int spoke, cell_size_t target )
			: a( nodeA ), b( nodeB ), s( spoke ), c( target )
		{}
		
		cell_size_t a, b, c;
		int s;
	};
}

#endif
