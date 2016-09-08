Small pathfinder library for 2d tilemaps	{#mainpage}
========================================

Inspired by a [tutorial by Patrick Lester](http://www.policyalmanac.org/games/aStarTutorial.htm), this library aims to be a slim and fast implementation of pathfinding routines for 2D tilemaps, primarily intended for use in games.

# DEPENDENCIES

Miscellaneous other libraries from same author, all included in this distribution and with same [license](@ref LICENSE.md). These are:

- `lash_tree3*.c` - Binary tree that stores generic objects. [Has its own repository on github](https://github.com/nolash/lashTree)
- `lash_debug*.c` - Simple timer and log functions for development and benchmarking.
- `lash_game_standard.c` - Common geometric and trigonometric functions used in 2D games
- `lash_game_map.c` - Simple layered tile map implementation

# BUILD

A Makefile is included in the project, with the following boilerplate options:

- `make static` - builds static libraries
- `make shared` - builds dynamic libraries
- `make test` - builds (and statically links) the applications for testing (and demonstrating) functionality

# EXAMPLES

Hints on intended usage can be found in the test/src directory.

### pathsimpledemo.c

Generates a random map, automatically determines the start and end points of the path and finds the path between.

It also contains a timer outputting amounts of ticks (`CLOCK_PROCESS_CPUTIME`) the pathfinding itself consumed, total and on average.

#### Usage

    pathsimpledemo_bin -w mapwidth -h mapheight -r movement_range -c test_runs
    pathsimpledemo_bin -f path_to_saved_mapfile -c test_runs

__movement_range__ clips a portion of the map equivalent to __movement_range * 2__ centered on the start location.

if __test_runs__ is not specified, the value in LASH_X_TEST_COUNT will be used.

The mapfile is a _little endian binary file_ with the following structure
    
    0x00 - 0x01 width (unsigned short int)
    0x02 - 0x03 height (unsigned short int)
    0x04 - 0x05 startindex (unsigned short int)
    0x06 - 0x07 startindex (unsigned short int)
    0x08 .... map entries (char)

Mapfiles can be generated from the ascii map visualizations the application outputs, with the included python tool. Save the text in a file called mapdata.txt in the same directory and run. It will output mapdata.dat (WARNING! on my system little-endian output produced, but no checks are in place to ensure that this is so).

# LICENSE

This software is released under the [GNU GENERAL PUBLIC LICENSE](@ref LICENSE.md)

# ROADMAP

## v0.3

- Implement graded modifiers between passable and impassable, enabling simulation of tough terrain.

# CHANGELOG

## v0.2

### Added

- Library is able to find way through binary defined map, either impassable or passable
- Test for autogeneration of map, endpoints and path, also with possibility of replaying saved map


