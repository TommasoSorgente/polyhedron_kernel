# polyhedron_kernel

Supplemental material for the paper "A Geometric Approach for Computing the Kernel of a Polyhedron" by T. Sorgente, S. Biasotti and M. Spagnuolo.

## Content
The repository contains the code for the computation of the geometric kernel of a polyhedron and the dataset considered in the paper for its testing.
- the folder "datasets" contains a .zip file for each type of mesh: _complex models_, _flex_ refinements, _laser-chess_ refinements, _tet10_, _tet20_, _tet30_ and _voro_. Each of them contains a number of .off meshes.
- kernel.pro is an example of Qt project for compiling main.cpp. The code is written using the library _cinolib_ (https://github.com/mlivesu/cinolib), which therefore needs to be included.
- main.cpp contains a basic usage example of the code: it takes a .off file as input, computes the kernel, saves it into another file and prints out the elapsed time.
- polyhedron_kernel.h/.cpp contains the kernel algorithm described in the paper.
- sort_points.h is an algorithm for sorting 2D points in clockwise order, needed by the function _polyhedron_plane_intersection_.

## Citing us
If you use the code or one or more datasets in your academic projects, please consider citing the original paper using the following BibTeX entry:

```
@misc{sorgente2021geometric,
      title={A Geometric Approach for Computing the Kernelof a Polyhedron}, 
      author={Tommaso Sorgente and Silvia Biasotti and Michela Spagnuolo},
      year={2021},
      eprint={2110.11054},
      archivePrefix={arXiv},
      primaryClass={cs.CG}
}
```

## Acknowldegment
This research has been supported bt the ERC Project CHANGE (https://cordis.europa.eu/project/id/694515), which has received funding from the European Research Council (ERC) under the European Union’s Horizon 2020 research and innovation programme (grant agreement No 694515).
