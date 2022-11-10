#pragma once

#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "statistics.h"

typedef struct Vector3D
{
    float x, y, z;
} Vector3D;

// Axis Aligned Bounding Box
typedef struct AABB
{
    Vector3D lower_bound, upper_bound;
} AABB;

typedef struct Triangle
{
    Vector3D a, b, c;
} Triangle;

typedef struct Node
{
    AABB bounding_box;
    int first_triangle_index;
    int tris_num;
    int left_child_index, right_child_index;
} Node;

// Bounding Volume Hierarchy
typedef struct BVH
{
    Node *nodes;
    Triangle *tris;
    int nodes_num, tris_num, used_nodes_num;
} BVH;

void swap_triangles(Triangle *a, Triangle *b)
{
    Triangle tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

float get_vec3_component(const Vector3D *v, int index)
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

AABB calc_triangle_aabb(const Triangle *t)
{
    AABB aabb;
    aabb.lower_bound.x = fminf(fminf(t->a.x, t->b.x), t->c.x);
    aabb.lower_bound.y = fminf(fminf(t->a.y, t->b.y), t->c.y);
    aabb.lower_bound.z = fminf(fminf(t->a.z, t->b.z), t->c.z);

    aabb.upper_bound.x = fmaxf(fmaxf(t->a.x, t->b.x), t->c.x);
    aabb.upper_bound.y = fmaxf(fmaxf(t->a.y, t->b.y), t->c.y);
    aabb.upper_bound.z = fmaxf(fmaxf(t->a.z, t->b.z), t->c.z);
    return aabb;
}

Vector3D calc_triangle_centroid(const Triangle *t)
{
    Vector3D centroid;
    centroid.x = (t->a.x + t->b.x + t->c.x) / 3.0f;
    centroid.y = (t->a.y + t->b.y + t->c.y) / 3.0f;
    centroid.z = (t->a.z + t->b.z + t->c.z) / 3.0f;
    return centroid;
}

void aabb_union(AABB *aabb, const AABB *other)
{
    vec3_min(&aabb->lower_bound, &other->lower_bound);
    vec3_max(&aabb->upper_bound, &other->upper_bound);
}

// bool is_leaf_node(const Node *node)
// {
//     return node->tris_num == 0;
// }

int last_triangle_index(const Node *node)
{
    return node->first_triangle_index + node->tris_num - 1;
}

BVH allocate_bvh(int tris_num)
{
    assert(tris_num > 0);
    BVH bvh;
    bvh.tris_num = tris_num;
    bvh.tris = (Triangle *)malloc(sizeof(Triangle) * tris_num);

    // Account for maximum possible number of binary tree nodes (2 * n - 1)
    bvh.nodes_num = tris_num * 2 - 1;
    bvh.nodes = (Node *)malloc(sizeof(Node) * bvh.nodes_num);

    bvh.used_nodes_num = 0;
    return bvh;
}

void free_bvh(BVH bvh)
{
    free(bvh.nodes);
    free(bvh.tris);
}

int get_new_node_index(BVH *bvh)
{
    return bvh->used_nodes_num++;
}

void build_tree_internal(BVH *bvh, int parent_node_index)
{
    Node *parent_node = bvh->nodes + parent_node_index;

    assert(parent_node->first_triangle_index >= 0);
    assert(parent_node->first_triangle_index < bvh->tris_num);
    assert(parent_node->first_triangle_index + parent_node->tris_num <= bvh->tris_num);

    // Calculate statistics used for splitting triangles array along the axis with most variance
    RunningStat triangles_stats_x = create_running_stat();
    RunningStat triangles_stats_y = create_running_stat();
    RunningStat triangles_stats_z = create_running_stat();

    {
        int first_index = parent_node->first_triangle_index;
        int last_index = last_triangle_index(parent_node);
        parent_node->bounding_box = calc_triangle_aabb(bvh->tris + first_index);
        for (int i = first_index + 1; i <= last_index; i++)
        {
            const Triangle *t = bvh->tris + i;
            // Calculate bounding box for speeding up tree traversal later on
            AABB tabb = calc_triangle_aabb(t);
            aabb_union(&parent_node->bounding_box, &tabb);

            update_stat(&triangles_stats_x, t->a.x);
            update_stat(&triangles_stats_y, t->a.y);
            update_stat(&triangles_stats_z, t->a.z);
        }
    }

    if (parent_node->tris_num < 2)
    {
        return;
    }

    float variance_x = get_variance(&triangles_stats_x);
    float variance_y = get_variance(&triangles_stats_y);
    float variance_z = get_variance(&triangles_stats_z);
    Vector3D variance = {variance_x, variance_y, variance_z};

    int split_axis = 0;
    if (variance.y > variance.x)
    {
        split_axis = 1;
    }
    if (variance.z > (split_axis == 0 ? variance.x : variance.y))
    {
        split_axis = 2;
    }

    RunningStat *split_axis_stats = NULL;

    if (split_axis == 0)
    {
        split_axis_stats = &triangles_stats_x;
    }
    else if (split_axis == 1)
    {
        split_axis_stats = &triangles_stats_y;
    }
    else
    {
        split_axis_stats = &triangles_stats_z;
    }

    float split_pos = get_mean(split_axis_stats);

    int i = parent_node->first_triangle_index;
    int j = i + parent_node->tris_num;

    while (i < j)
    {
        Triangle *t = bvh->tris + i;
        Vector3D t_centroid = calc_triangle_centroid(t);
        if (get_vec3_component(&t_centroid, split_axis) < split_pos)
        {
            i++;
        }
        else
        {
            assert(j != 0);
            j--;
            swap_triangles(bvh->tris + i, bvh->tris + j);
        }
    }

    int left_node_first_triangle_index = parent_node->first_triangle_index;
    int left_node_tris_num = i - left_node_first_triangle_index;

    if (left_node_tris_num == 0 || left_node_tris_num == parent_node->tris_num)
    {
        return; // Failed to partition, abort BVH node split
    }

    int right_node_first_triangle_index = i;
    int right_node_tris_num = parent_node->tris_num - left_node_tris_num;

    parent_node->tris_num = 0;

    parent_node->left_child_index = get_new_node_index(bvh);
    Node *left_node = bvh->nodes + parent_node->left_child_index;
    left_node->first_triangle_index = left_node_first_triangle_index;
    left_node->tris_num = left_node_tris_num;

    parent_node->right_child_index = get_new_node_index(bvh);
    Node *right_node = bvh->nodes + parent_node->right_child_index;
    right_node->first_triangle_index = right_node_first_triangle_index;
    right_node->tris_num = right_node_tris_num;

    build_tree_internal(bvh, parent_node->left_child_index);
    build_tree_internal(bvh, parent_node->right_child_index);
}

void build_tree(BVH *bvh)
{
    // Setup root node
    bvh->nodes[0].first_triangle_index = 0;
    bvh->nodes[0].tris_num = bvh->tris_num;
    // Subdivide root node recursively
    build_tree_internal(bvh, 0);
}