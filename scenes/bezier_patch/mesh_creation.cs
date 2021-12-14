#version 460

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform int PATCH_WIDTH;

struct MeshNode {
    vec3 pos;
    float pad;
};

layout (std430, binding = 1) buffer patch_nodes_buf
{
    MeshNode patch_nodes[];
};

layout (std430, binding = 2) buffer points_buf
{
    MeshNode points[];
};

void create_triangles(int x, int y)
{
    MeshNode a = patch_nodes[x + PATCH_WIDTH * y];
    MeshNode b = patch_nodes[x + PATCH_WIDTH * (y + 1)];
    MeshNode c = patch_nodes[(x + 1) + PATCH_WIDTH * (y + 1)];
    MeshNode d = patch_nodes[(x + 1) + PATCH_WIDTH * y];

    vec3 u = b.pos - a.pos;
    vec3 v = b.pos - c.pos;
    MeshNode normal;
    normal.pos = normalize(cross(u, v));

    int points_idx = 2 * 6 * (x + y * PATCH_WIDTH);

    // First triangle
    points[points_idx++] = a;
    points[points_idx++] = normal;
    points[points_idx++] = b;
    points[points_idx++] = normal;
    points[points_idx++] = c;
    points[points_idx++] = normal;

    // Second triangle
    points[points_idx++] = a;
    points[points_idx++] = normal;
    points[points_idx++] = c;
    points[points_idx++] = normal;
    points[points_idx++] = d;
    points[points_idx++] = normal;
}

void main () {
    create_triangles(int(gl_GlobalInvocationID.x), int(gl_GlobalInvocationID.y));
}