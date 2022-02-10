#include "polyhedron_kernel.h"
#include <chrono>
#include <cinolib/meshes/meshes.h>

using namespace cinolib;

int main(int argc, char *argv[]) {
  std::string input =
      (argc == 2) ? std::string(argv[1])
                  : std::string(DATA_PATH) + "complex_models/rt4_arm.off";
  std::cout << "Input: " << input << std::endl;
  Polygonmesh<> m(input.c_str());

  auto start = std::chrono::steady_clock::now();

  PolyhedronKernel K;
  K.initialize(m.vector_verts());
  K.compute(m.vector_verts(), m.vector_polys(), m.vector_poly_normals());

  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now() - start);

  Polygonmesh<> kernel(K.kernel_verts, K.kernel_faces);
  std::cout << "Kernel: " << kernel.num_verts() << " verts, "
            << kernel.num_polys() << " faces" << std::endl
            << "Elapsed time: " << time.count() << " ms" << std::endl;

  input.erase(input.end() - 4, input.end());
  std::string output = input + "_kernel.off";
  kernel.save(output.c_str());
  std::cout << "Saved in: " << output << std::endl;
}
