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

Generate a basic geodesic grid with 8 subdivisions and name it `test_grid`:

`zawarudo -w test_grid -i 8`

Use `test_grid` as a base and create height data and save as `test_terrain`:

`zawarudo -w test_terrain --base test_grid -i 8 -p 10000`

Rescale 'test_terrain' to Earth-like dimensions and hydrographics:

`zawarudo -w test_terrain -i 8 -R 6371 -H 70`

Create orthographic projections of front and back hemispheres:

`zawarudo -w test_terrain -i 8 -m orthographic`
`zawarudo -w test_terrain -i 8 -m orthographic --meridian 180`

Create equirectangular projections with 45-degree standard parallel:

`zawarudo -w test_terrain -i 8 --parallel 45`

