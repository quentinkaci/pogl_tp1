#version 460

const int NB_PARTICLES = 100;
int MAX_LIFE_TIME = 100;
uint rng_state;
const float M_PI = 3.14;

layout (local_size_x = 1024) in;

struct Particle {
    vec3 pos;
    float size;
    vec3 pad;
    int life_time;
};

layout (std430, binding = 1) buffer particle_pos_buffer
{
    Particle particles[NB_PARTICLES];
};

float rand()
{
    // Xorshift algorithm from George Marsaglia's paper
    rng_state ^= (rng_state << 13);
    rng_state ^= (rng_state >> 17);
    rng_state ^= (rng_state << 5);
    return float(rng_state) * (1.0 / 4294967296.0);
}

void init_particle(int particle_index) {
    float r = rand();
    float y = rand();
    float theta = rand();
    float t = rand();

    particles[particle_index].life_time = (MAX_LIFE_TIME - 1 - int(t * 50.0));
    particles[particle_index].pos.x = r * 0.1 * cos(theta * 2.0 * M_PI) + rand() * 20.0 - 10.0;
    particles[particle_index].pos.y = -y * 10.0 - 10.0;
    particles[particle_index].pos.z = r * 0.1 * sin(theta * 2.0 * M_PI);

    particles[particle_index].size = 0.1;
}

void update_particle(int i) {
    particles[i].life_time -= 1;
    if (particles[i].life_time <= 0) {
        init_particle(i);
    }
    else {
        float x = rand();
        float y = rand();
        float z = rand();

        particles[i].pos.x += (x - 0.5) * 1.0;
        particles[i].pos.y += y;
        particles[i].pos.z += (z - 0.5) * 1.0 + 0.05;
        particles[i].size += 0.05;
    }
}

void main () {
    update_particle(int(gl_GlobalInvocationID.x));
}