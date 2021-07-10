#version 460

uniform sampler2D texture_sampler;

in vec2 uv;
in vec4 normal;
in vec4 light_dir;

out vec4 output_color;

vec4 fog_color = vec4(1.0, 1.0, 1.0, 1.0);

float get_fog_factor(float d) {
    const float FOG_MIN =  0.925;
    const float FOG_MAX = 1.0;
    if (d >= FOG_MAX) return 1.0;
    if (d <= FOG_MIN) return 0.0;

    return 1 - (FOG_MAX - d) / (FOG_MAX - FOG_MIN);
}

void main() {
    float fog = get_fog_factor(gl_FragCoord.z);
    vec4 initial_color = vec4(dot(normal, light_dir) * texture(texture_sampler, uv).rgb, 1.f);
    output_color = mix(initial_color, fog_color, fog);
}