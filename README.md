# TORCS Adaptive
TORCS Adaptive is a work-in-progress modification to The Open Source Racing Car Simulator (TORCS). Currently, the modification has fully
functional procedural track generation within the TORCS engine, though some track parameters are omitted and it could do with some visual
improvements (terrain, for example).

## Planned Features
Features currently being worked on are:
* Player and bot Performance Measurement in TORCS.
* Adaptive Procedural Tracks that adapt to measured player or bot performance.

## Current Features
Currently, TORCS-Adaptive features an easily extensible performance measurement library that can be used to track a player in a traditional
TORCS track or a TORCS-Adaptive procedural track. This means that it is easy to implement bespoke performance measurement behaviours into
the existing source code. Currently, some simple racing line evaluation methods are being worked on.

## Links and other Information
TORCS can be found at: http://torcs.sourceforge.net/

torcs-adaptive is inspired by previous work, including the existing evolutionary track-generator for TORCS:http://trackgen.pierlucalanzi.net/index.php

torcs-adaptive is a Final Year Undergraduate Project currently being carried out at the University of Lincoln - UK.

More information can be found on the developer's personal blog: http://hk-gamedev.blogspot.co.uk/search/label/TORCS
