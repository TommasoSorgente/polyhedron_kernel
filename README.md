# polyhedron_kernel

Supplemental material for the paper "Polyhedron Kernel Computation Using a Geometric Approach" by T. Sorgente, S. Biasotti and M. Spagnuolo.

## How to get it 

Please use --recursive when cloning this repository:

```
git clone --recursive git@github.com:TommasoSorgente/polyhedron_kernel.git
```
polyhedron_kernel can be built through CMake by running the following instructions.
The executable will be available in `${REPO_ROOT}/build`, being `${REPO_ROOT}` the folder where this `README.md` lies. 

```
cd ${REPO_ROOT}
mkdir build
cd build
cmake ..
make
```

## Content
The repository contains the code for the computation of the geometric kernel of a polyhedron and the dataset considered in the paper for its testing.
- the folder "datasets" contains .zip files for each type of mesh: in _Refinements_, there are the _vase_ and _spiral_ refinements; in _Thingi_ the subset of _Thingi10K_ considered in the paper; in _ComplexModels_ the 10 particular models from _Thingi_ analyzed in Section 4.3. Each of them contains a number of .off meshes. Note that meshes from Section 4.1 are available at https://github.com/TommasoSorgente/vem-indicator-3D-dataset.
- main.cpp contains a basic usage example of the code: it takes a .off file as input, computes the kernel, saves it into another file and prints out the elapsed time.
- polyhedron_kernel.h/.cpp contains the kernel algorithm described in the paper.
- sort_points.h is an algorithm for sorting 2D points in clockwise order, needed by the function _polyhedron_plane_intersection_.
- extendedplane.h is the extended version of the cinolib::plane class, with the additional information of three points contained in the plane, useful for Shewchuck predicates.

## Citing us
If you use the code or one or more datasets in your academic projects, please consider citing the original paper using the following BibTeX entry:

```
@article{sorgente2022polyhedron,
  title={Polyhedron kernel computation using a geometric approach},
  author={Sorgente, Tommaso and Biasotti, Silvia and Spagnuolo, Michela},
  journal={Computers \& Graphics},
  volume={105},
  pages={94--104},
  year={2022},
  publisher={Elsevier}
}
```

## Acknowldegment
This research has been supported bt the ERC Project CHANGE (https://cordis.europa.eu/project/id/694515), which has received funding from the European Research Council (ERC) under the European Union’s Horizon 2020 research and innovation programme (grant agreement No 694515).
