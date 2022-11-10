#include <stdio.h>

#include "bvh.h"
#include "read_stl.h"

int main()
{
    BinarySTLMesh mesh = read_binary_stl_mesh("/home/iyad/Downloads/STL/engine.stl");
    printf("Number of Triangles = %d\n", mesh.tris_num);
    BVH bvh = allocate_bvh(mesh.tris_num);
    for (uint32_t i = 0; i < mesh.tris_num; i++)
    {
        bvh.tris[i] = *(Triangle *)(mesh.tris + i);
    }
    build_tree(&bvh);
    printf("Number of BVH Nodes = %d\n", bvh.nodes_num);
    free_bvh(&bvh);
    free_binary_stl_mesh(&mesh);
    return 0;
}