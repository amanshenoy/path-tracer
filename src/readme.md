## Notes on source code

This directory contains all the abstractions for the rendering engine. `src/dependancies` contains a few notes on getting the dependancies in the right format. Generally the source code has been structured into 5 sub-folders. 

* `core` sub-folder and namespace contains the core elements of the path tracer
* `optim` sub-folder and namespace contains the bounding volume heirarchy implementation (k-D tree of sorts)
* `shaders` sub-folder and `bsdf` namespace contains the disney bsdf and its utilities
* `utils` sub-folder and namespace contains general utilities, and contains sub-namespace `utils::sampler`, which samples random numbers within various domains (can use PRNG of choice, but I just used the default C `rand()` and intialized seed with time)
* `scenes` contains all the implementations of the various scenes shown in the main readme. Most of them are extremely messy, and I would recommend looking at `David.scene.h` or `VaryingFuzz.scene.h` as they are written far more cleanly then the rest.   
* `dependancies/` should be linked as submodules