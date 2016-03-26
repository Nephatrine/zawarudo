# zawarudo

This will eventually be a world generator and simulation tool. I'm aiming more
for versimilitude than accuracy as this is being used for gaming and writing
rather than any sort of actual science.

Right now this tool is still extremely basic (and completely unoptimized) and
has no point.

## Installation

`mkdir build`
`cd build`
`cmake ..`
`make && make install`

## Usage

See complete usage instructions:

`zawarudo --help`

### Create Geodesic Grid

Generate a basic geodesic grid with 8 subdivisions and name it `test_grid`.
Generating geodesics with greater numbers of subdivisions takes progressively
longer. At 10 subdivisions it takes 20-30 seconds usually.

`zawarudo -i 8 -w geodesic`

This generates a flat grid named `geodesic_8.dat`.

### Create Heightmap

The geodesic grid created above is an approximation of a flat sphere and so
doesn't have any elevation differences aside from those that arise from
floating-point precision issues.

This command will use 3D coherent noise to create a varied heightmap for the
geodesic grid using fractal brownian motion and mutli-ridged fractal noise. This
generates a heightmap with some ridge-like features to represent ocean trenches
and mountain ridges. **The ridges are still experimental and being tweaked.**

`zawarudo -i 8 -n -r -w geodesic`

That loads our previous `geodesic_8.dat` file and creates the base heightmap. It then saves the grid to the original file. Be cautious because if you run this multiple times in a row as distorting an already-distorted heightmap will introduce strange exaggerated formations.

Here's a planet using just FBM noise (`-n`):

![Perlin Planet](http://i.imgur.com/MthQUTN.png)

Here's a planet with added ridged noise (`-r`):

![Ridged Planet](http://i.imgur.com/JpnYK2Z.png)

### Scale To Planet

Planets aren't just randomized spheres. There are limits on the height of
peaks, depth of trenches, etc. Our geodesic grid so far is also based on a unit
sphere currently where we'd prefer to work with other units such as kilometers.

We'll use our heightmap terrain as a base so we don't need to wait through the
perturbation process again.

This command will take our heightmap data and scale it to Earth-size with 70%
hydrographic coverage.

`zawarudo -i 8 -R 6371 -H 70 -w terran --base geodesic`

**IMPORTANT:** Once you add a hydrographic coverage to a world, any further
changes to the hydrographic coverage (or heightmap) will not preserve the slope
of the land the same way.

### Create Maps

Create orthographic projections of front and back hemispheres:

`zawarudo -w terran -i 8 -m orthographic`
`zawarudo -w terran -i 8 -m orthographic --meridian 180`

Create equirectangular projections with 45-degree standard parallel:

`zawarudo -w terran -i 8 --parallel 45`

