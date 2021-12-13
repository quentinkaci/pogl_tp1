#version 460

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct MeshNode {
    vec3 pos;
};

layout (std430, binding = 1) buffer mesh_nodes_buf
{
    MeshNode mesh_nodes[];
};

layout (std430, binding = 2) buffer patch_nodes_buf
{
    MeshNode patch_nodes[];
};

float BezierBlend(int k, float mu, int n)
{
    float blend = 1;

    int nn = n;
    int kn = k;
    int nkn = n - k;

    while (nn >= 1) {
        blend *= nn;
        --nn;
        if (kn > 1) {
            blend /= float(kn);
            --kn;
        }
        if (nkn > 1) {
            blend /= float(nkn);
            --nkn;
        }
    }

    if (k > 0)
        blend *= pow(mu, float(k));
    if (n - k > 0)
        blend *= pow(1 - mu, float(n - k));

    return blend;
}

const int MESH_HEIGHT = 5;
const int MESH_WIDTH = 4;

const int PATCH_HEIGHT = 10 * MESH_HEIGHT;
const int PATCH_WIDTH = 10 * MESH_WIDTH;

void compute_bezier_patch(int x, int y)
{
    int patch_idx = x + PATCH_WIDTH * y;

    float mu_y = y / float(PATCH_HEIGHT - 1);
    float mu_x = x / float(PATCH_WIDTH - 1);

    patch_nodes[patch_idx].pos.x = 0;
    patch_nodes[patch_idx].pos.y = 0;
    patch_nodes[patch_idx].pos.z = 0;

    for (int k_y = 0; k_y <= MESH_HEIGHT; ++k_y) {
        float b_y = BezierBlend(k_y, mu_y, MESH_HEIGHT);

        for (int k_x = 0; k_x <= MESH_WIDTH; ++k_x) {
            float b_x = BezierBlend(k_x, mu_x, MESH_WIDTH);

            int mesh_idx = k_x + MESH_WIDTH * k_y;

            patch_nodes[patch_idx].pos.x += (mesh_nodes[mesh_idx].pos.x * b_y * b_x);
            patch_nodes[patch_idx].pos.y += (mesh_nodes[mesh_idx].pos.y * b_y * b_x);
            patch_nodes[patch_idx].pos.z += (mesh_nodes[mesh_idx].pos.z * b_y * b_x);
        }
    }
}

void main () {
    compute_bezier_patch(int(gl_GlobalInvocationID.x), int(gl_GlobalInvocationID.y));
}