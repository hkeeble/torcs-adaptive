# TORCS Adaptive
TORCS Adaptive is a work-in-progress modification to The Open Source Racing Car Simulator (TORCS). It is a final year undergraduate project currently being carried out
at the University of Lincoln - UK. The modification has fully functional procedural track generation within the TORCS engine, though some track parameters are omitted
and it could do with some visual improvements (terrain, for example).

## Features
* Procedural Track Generation
* Player and AI Driver performance measurement library.
* Race Line Evaluation performance evaluator.
* A procedural pathfinder, adapted from the pathfinder included with the Berniw robot distributed with TORCS.
* Three AI Drivers that make use of the procedural pathfinder.
* Able to save/reload procedurally generated tracks.

## Possible Improvements
The following is a list of possible improvements to the current build of TORCS Adaptive.
* Improved track segment parameters (e.g, elevation, banking, sharper corners).
* Terrain Generation for procedural tracks.
* Object/Scenery Generation for procedural tracks.

## Building
To Build TORCS Adaptive, you'll need Visual Studio C++ 2012 or 2013 (if you're using 2013,
you'll also need a copy of the MSVC11.0 compiler installed, as this is what TORCS Adaptive compiles with).

Once you've downloaded a copy of the source, run setup.bat to set up a runtime environment and copy all necessary data files
for either a release or debug build. Afterwards, open up Visual Studio and you will be able to build.

## Releases
* [v0.5 Pre-Release](https://github.com/hkeeble/torcs-adaptive/releases/tag/v0.5): This pre-release shows most of the TORCS Adaptive features having been implemented.

## Links and other Information
TORCS can be found at: http://torcs.sourceforge.net/

torcs-adaptive is inspired by previous work, including the existing evolutionary track-generator for TORCS:http://trackgen.pierlucalanzi.net/index.php

More information can be found on the developer's personal blog: http://hk-gamedev.blogspot.co.uk/search/label/TORCS
