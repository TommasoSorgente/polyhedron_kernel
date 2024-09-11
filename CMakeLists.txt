cmake_minimum_required(VERSION 3.5)

project(polyhedron_kernel)

add_executable(${PROJECT_NAME} main.cpp)

set(cinolib_DIR ${PROJECT_SOURCE_DIR}/cinolib)
set(CINOLIB_USES_SHEWCHUK_PREDICATES OFF)
find_package(cinolib REQUIRED)

target_link_libraries (${PROJECT_NAME} PUBLIC cinolib)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

add_compile_definitions(DATA_PATH="${PROJECT_SOURCE_DIR}/datasets/")
