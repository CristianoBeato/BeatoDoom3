#!/bin/bash

# This script compiles the shader files in the current directory and outputs them to the specified output directory.


#input_dir="./glsl/"
#output_dir="./spirv/"


rm -rf ./spirv/*.spv

# shader compile flags
COMPILE_VERTEX_FLAGS="-O -fshader-stage=vertex"
COMPILE_FRAGMENT_FLAGS="-O --target-spv=spv1.0 -fshader-stage=fragment"

# compile vertex color shaders
glslc "./glsl/vertexcolor.vert" -o "./spirv/vertexcolor_vert.spv" ${COMPILE_VERTEX_FLAGS}
glslc "./glsl/vertexcolor.frag" -o "./spirv/vertexcolor_frag.spv" ${COMPILE_FRAGMENT_FLAGS}

# compile texturecolor shaders
glslc "./glsl/texturecolor.vert" -o "./spirv/texturecolor_vert.spv" ${COMPILE_VERTEX_FLAGS}
glslc "./glsl/texturecolor.frag" -o "./spirv/texturecolor_frag.spv" ${COMPILE_FRAGMENT_FLAGS}