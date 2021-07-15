#version 460

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

uniform sampler2D texture_sampler;
uniform sampler2DShadow shadowMap;

in vec2 uv;
in vec4 shadow_coord;

out vec4 output_color;

void main() {
	vec3 LightColor = vec3(1, 1, 1);
	
	vec3 MaterialDiffuseColor = texture(texture_sampler, uv).rgb;

    float bias = 0.005;
    float visibility = 1.0;
    if (texture(shadowMap, vec3(shadow_coord.xy, 1)) < shadow_coord.z - bias)
    {
        visibility = 0.2;
    }

	//float visibility = texture(shadowMap, vec3(shadow_coord.xy, shadow_coord.z / shadow_coord.w));

	output_color = vec4(visibility * MaterialDiffuseColor * LightColor, 1);
}