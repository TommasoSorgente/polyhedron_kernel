# polyhedron_kernel

Supplemental material for the paper "A Geometric Approach for Computing the Kernel of a Polyhedron" by T. Sorgente, S. Biasotti and M. Spagnuolo.

## Content
The repository contains the code for the computation of the geometric kernel of a polyhedron and the dataset considered in the paper for its testing.
- the folder "datasets" contains a .zip file for each type of meshes: _complex models_, _flex refinements_, _laser-chess_ refinements, _tet10_, _tet20_, _tet30_ and _voro_. Each of them contains a number of .off meshes.
- example.cpp contains a basic usage example of the code.
- polyhedron_kernel.h/.cpp contains the core of the kernel algorithm described in the paper.
- sort_points.h is an algorithm for sorting 2D points in clockwise order.
- polyhedron_utils.h/.cpp contains useful functions for the handling of polyhedra which are not included in the curent version of _cinolib_.
- qhull_wrap.h/.cpp is a wrapper to the _qhull_ library.

## Citing us
If you use the code or one or more datasets in your academic projects, please consider citing the original paper using the following BibTeX entry:

```
@misc{sorgente2021role,
      title={A Geometric Approach for Computing the Kernel of a Polyhedron}, 
      author={Tommaso Sorgente and Silvia Biasotti and Michela Spagnuolo},
      year={2021},
}
```

## Acknowldegment
This research has been supported bt the ERC Project CHANGE (https://cordis.europa.eu/project/id/694515), which has received funding from the European Research Council (ERC) under the European Union’s Horizon 2020 research and innovation programme (grant agreement No 694515).
