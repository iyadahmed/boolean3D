#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#pragma pack(push, 1)
typedef struct BinarySTLMeshTriangle
{
    float custom_normal[3];
    float vertices[3][3];
    uint16_t attribute_byte_count;
} BinarySTLMeshTriangle;
#pragma pack(pop)

typedef struct BinarySTLMesh
{
    BinarySTLMeshTriangle *tris;
    uint32_t tris_num;
} BinarySTLMesh;

void free_binary_stl_mesh(BinarySTLMesh mesh)
{
    free(mesh.tris);
}

bool is_null_binary_stl_mesh(BinarySTLMesh mesh)
{
    return mesh.tris == NULL;
}

void print_file_error(FILE *file)
{
    if (ferror(file))
        puts("Error reading file");
    else if (feof(file))
        puts("EOF found");
    else
        puts("Unknown file error");
}

// fread wrapper with error checking
void fread_e(void *output, size_t size, size_t n, FILE *file)
{
    size_t num_read_items = fread(output, size, n, file);
    if (num_read_items != n)
    {
        print_file_error(file);
        exit(1);
    }
}

BinarySTLMesh read_binary_stl_mesh(const char *filepath)
{
    uint8_t header[80];
    BinarySTLMesh mesh = {NULL, 0};

    FILE *file = fopen(filepath, "rb");
    if (file == NULL)
        return mesh;

    fread_e(header, sizeof(header), 1, file);
    fread_e(&mesh.tris_num, sizeof(mesh.tris_num), 1, file);
    mesh.tris = malloc(mesh.tris_num * sizeof(BinarySTLMeshTriangle));
    fread_e(mesh.tris, mesh.tris_num * sizeof(BinarySTLMeshTriangle), 1, file);
    fclose(file);
    return mesh;
}