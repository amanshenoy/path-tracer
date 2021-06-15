cmake_minimum_required (VERSION 3.16)

project (Path-Tracer)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -std=c++17")
set (source_dir "${PROJECT_SOURCE_DIR}/src/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

FIND_LIBRARY(FreeImage_LIBRARY NAMES FreeImage HINTS ${PROJECT_SOURCE_DIR}/src/dependancies)
#FIND_LIBRARY(OpenImageDenoise NAMES OpenImageDenoise HINTS ${PROJECT_SOURCE_DIR}/src/dependancies)

find_package(OpenMP)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

file (GLOB source_files "${source_dir}/*.cpp" "${source_dir}/dependancies/*.cpp" "${source_dir}/dependancies/*/*.cpp" "${source_dir}/core/*.cpp" "${source_dir}/optim/*.cpp" "${source_dir}/utils/*.cpp" "${source_dir}/shaders/*.cpp")

add_executable (path-tracer ${source_files})
target_link_libraries(path-tracer freeimage)
