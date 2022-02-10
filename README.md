# polyhedron_kernel

Supplemental material for the paper "Polyhedron Kernel Computation Using a Geometric Approach" by T. Sorgente, S. Biasotti and M. Spagnuolo.

## Content
The repository contains the code for the computation of the geometric kernel of a polyhedron and the dataset considered in the paper for its testing.
- the folder "datasets" contains .zip files for each type of mesh: in _Refinements_, there are the _vase_ and _spiral_ refinements; in _Thingi_ the subset of _Thingi10K_ considered in the paper; in _ComplexModels_ the 10 particular models from _Thingi_ analyzed in Section 4.3. Each of them contains a number of .off meshes. Note that meshes from Section 4.1 are available at https://github.com/TommasoSorgente/vem-indicator-3D-dataset.
- kernel.pro is an example of Qt project for compiling main.cpp. The code is written using the library _cinolib_ (https://github.com/mlivesu/cinolib), which therefore needs to be included. The code has been developed on the commit hash f7d14627f353e37e8a5c6c7c2bf442cc956a4a8b of 25 October 2021.
- main.cpp contains a basic usage example of the code: it takes a .off file as input, computes the kernel, saves it into another file and prints out the elapsed time.
- polyhedron_kernel.h/.cpp contains the kernel algorithm described in the paper.
- sort_points.h is an algorithm for sorting 2D points in clockwise order, needed by the function _polyhedron_plane_intersection_.
- extendedplane.h is the extended version of the cinolib::plane class, with the additional information of three points contained in the plane, useful for Shewchuck predicates.

## Citing us
If you use the code or one or more datasets in your academic projects, please consider citing the original paper using the following BibTeX entry:

```
@misc{
}
```

## Acknowldegment
This research has been supported bt the ERC Project CHANGE (https://cordis.europa.eu/project/id/694515), which has received funding from the European Research Council (ERC) under the European Union’s Horizon 2020 research and innovation programme (grant agreement No 694515).
