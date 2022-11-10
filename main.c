#include <stdio.h>

#include "bvh.h"
#include "read_stl.h"


int main()
{
    BinarySTLMesh mesh = read_binary_stl_mesh("/home/iyad/Downloads/STL/engine.stl");
    printf("Number of Triangles = %d\n", mesh.tris_num);
    BVH bvh = allocate_bvh(mesh.tris_num);
    return 0;
}