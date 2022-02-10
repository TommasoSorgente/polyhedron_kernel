#include <chrono>
#include <cinolib/meshes/meshes.h>
#include <polyhedron_kernel.h>

using namespace cinolib;

int main(int argc, char *argv[]) {
  std::string input =
      (argc == 2) ? std::string(argv[1])
                  : std::string(DATA_PATH) + "complex_models/rt4_arm.off";
  std::cout << "Input: " << input << std::endl;
  Polygonmesh<> m(input.c_str());

  auto start = std::chrono::steady_clock::now();

  std::vector<vec3d> kernel_verts;
  std::vector<std::vector<uint>> kernel_faces;
  assert(polyhedron_kernel(m.vector_verts(), m.vector_polys(),
                           m.vector_poly_normals(), kernel_verts,
                           kernel_faces));

  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::steady_clock::now() - start);

  Polygonmesh<> kernel(kernel_verts, kernel_faces);
  std::cout << "Kernel: " << kernel.num_verts() << " verts, "
            << kernel.num_polys() << " faces" << std::endl
            << "Elapsed time: " << time.count() << " ms" << std::endl;

  input.erase(input.end() - 4, input.end());
  std::string output = input + "_kernel.off";
  kernel.save(output.c_str());
  std::cout << "Saved in: " << output << std::endl;
}
