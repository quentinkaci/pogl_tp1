#version 460

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform int MESH_HEIGHT;
uniform int MESH_WIDTH;

uniform int PATCH_HEIGHT;
uniform int PATCH_WIDTH;

struct MeshNode
{
    vec3 pos;
    float pad;
};

layout (std430, binding = 1) buffer mesh_nodes_buf
{
    MeshNode mesh_nodes[];
};

layout (std430, binding = 2) buffer patch_nodes_buf
{
    MeshNode patch_nodes[];
};

int factorial(int n)
{
    int res = 1;

    while (n > 1)
        res *= n--;

    return res;
}

float bernstein_polynomial(int n, int i)
{
    return factorial(n) / (factorial(i) * factorial(n - i));
}

float bezier_coef(int i, float u, int n)
{
    return bernstein_polynomial(n, i) * pow(u, float(i)) * pow(1 - u, float(n - i));
}

void compute_bezier_patch(int x, int y)
{
    int patch_idx = x + PATCH_WIDTH * y;

    float u = 1.0 * y / (PATCH_HEIGHT - 1);
    float v = 1.0 * x / (PATCH_WIDTH - 1);

    for (int i = 0; i <= MESH_HEIGHT; ++i) {
        float bi = bezier_coef(i, u, MESH_HEIGHT);

        for (int j = 0; j <= MESH_WIDTH; ++j) {
            float bj = bezier_coef(j, v, MESH_WIDTH);

            int mesh_idx = j + (MESH_WIDTH + 1) * i;

            patch_nodes[patch_idx].pos.x += mesh_nodes[mesh_idx].pos.x * bi * bj;
            patch_nodes[patch_idx].pos.y += mesh_nodes[mesh_idx].pos.y * bi * bj;
            patch_nodes[patch_idx].pos.z += mesh_nodes[mesh_idx].pos.z * bi * bj;
        }
    }
}

void main ()
{
    compute_bezier_patch(int(gl_GlobalInvocationID.x), int(gl_GlobalInvocationID.y));
}