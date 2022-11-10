#pragma once

typedef struct Vector3D
{
    float x, y, z;
} Vector3D;

float get_vec3_component_by_index(const Vector3D *v, int index)
{
    const float *arr = &(v->x);
    return arr[index];
}

void vec3_max(Vector3D *a, const Vector3D *b)
{
    a->x = a->x > b->x ? a->x : b->x;
    a->y = a->y > b->y ? a->y : b->y;
    a->z = a->z > b->z ? a->z : b->z;
}

void vec3_min(Vector3D *a, const Vector3D *b)
{
    a->x = a->x < b->x ? a->x : b->x;
    a->y = a->y < b->y ? a->y : b->y;
    a->z = a->z < b->z ? a->z : b->z;
}