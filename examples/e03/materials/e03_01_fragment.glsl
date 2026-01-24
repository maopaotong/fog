#version 330 core

uniform sampler2D tex_elevation;
uniform sampler2D tex_colormap;
uniform float ambient;

const float biome_colors = 1;
const vec3 neutral_land_biome = vec3(0.9, 0.8, 0.7);
const vec2 inverse_texture_size = vec2(1.5 / 1920.0, 1.5 / 1080.0);
const float overhead = 30;
const float light_angle_rad = 3.1415926 * 80.0 / 180.0;
const vec2 light_angle = vec2(cos(light_angle_rad), sin(light_angle_rad));
const float slope = 2;
const float flat_ = 2.5;
in float fE;
in vec2 fPos;
out vec4 color;


void main() {

    vec2 dx = vec2(inverse_texture_size.x, 0);
    vec2 dy = vec2(0, inverse_texture_size.y);

    float z = texture2D(tex_elevation, fPos).r;
    float zE = texture2D(tex_elevation, fPos + dx).r;
    float zN = texture2D(tex_elevation, fPos - dy).r;
    float zW = texture2D(tex_elevation, fPos - dx).r;
    float zS = texture2D(tex_elevation, fPos + dy).r;
    vec3 slope_vector = normalize(vec3(zS-zN, zE-zW, overhead * (inverse_texture_size.x + inverse_texture_size.y)));
    vec3 light_vector = normalize(vec3(light_angle, mix(slope, flat_, slope_vector.z)));

    vec3 neutral_biome_color = neutral_land_biome;
    float m = 0.5;
    //vec3 biome_color = texture2D(tex_colormap, vec2(fE, m)).rgb;
    vec3 biome_color = texture2D(tex_colormap, vec2(fE, m)).rgb;
    biome_color = mix(neutral_biome_color, biome_color, biome_colors);

    float light = ambient + max(0.0, dot(light_vector, slope_vector));    
    color = vec4(biome_color * light, 1.0);
    //color = vec4(z , 0,0,1);
}