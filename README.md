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

This command takes the geodesic we created earlier and perturbs the points that
make up the geodesic shape. This uses a slicing process that needs to run a
large number of iterations to generate a pleasing result.

The number of perturbation passes and number of subdivision passes both will
increase the time this takes dramatically. Running the preferred 10,000 perturb
passes on a geodesic with 8 subdivisions takes over 5 minutes.

`zawarudo -i 8 -n -p 10000 -w geodesic`

That loads `geodesic_8.dat`, seeds it with Perlin noise,  and runs 10,000
perturbation passes on it. This will take a long time at 8+ subdivision
iterations. It then saves the generated grid to the original file.

Here's a planet using just Perlin noise (`-n`):

![Perlin Planet](http://i.imgur.com/RqUEOuw.png)

Here's a planet using just the slicing method (`-p 10000`):

![Sliced Planet](http://i.imgur.com/ssoYq4Q.png)

Here's one with both selected:

![Hybrid Planet](http://i.imgur.com/uPJk9Ag.png)

Using both options tends to preserve the shape of the Perlin noise, but with much greater detail.

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
changes to the hydrographic coverage will not preserve the slope of the land
correctly. If you need to change the coverage at a later point, it is suggested
to run the command using a backed up grid that you did not already apply it to.

### Create Maps

Create orthographic projections of front and back hemispheres:

`zawarudo -w terran -i 8 -m orthographic`
`zawarudo -w terran -i 8 -m orthographic --meridian 180`

Create equirectangular projections with 45-degree standard parallel:

`zawarudo -w terran -i 8 --parallel 45`

