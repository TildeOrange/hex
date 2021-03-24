#ifdef _VERTEX_

in vec3 vertex_position;
in vec3 vertex_normal;
in vec3 instance_position;
// in vec3 color;

layout(location = 0) out vec3 fcolor;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 color;
uniform float time;
uniform int base_index;
uniform int selected_instance;

const vec3 sun_dir = vec3(1, 2, 1);

float rand(vec2 co){
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

void main(void)
{
    gl_Position = proj * view * vec4(vertex_position + instance_position, 1); 

    float light_intensity = clamp(dot(normalize(sun_dir), vertex_normal) * 0.5 + 0.5, 0.25, 1.0);
    float rand_intensity = 1 - rand(instance_position.xz) / 10.0;
    float depth_intensity = clamp((2 + vertex_position.y + instance_position.y) / 2.0, 0, 1);

    fcolor = color * rand_intensity * light_intensity * depth_intensity;

    // highlight in draw order
    // if (base_index + gl_InstanceID == selected_instance)
        // fcolor = vec3(0.5, 0.25, 1) * (0.5 + 0.5 * sin(time * 7.5));
}

#endif // _VERTEX_

//=============================================================================

#ifdef _FRAGMENT_

layout(location = 0) in vec3 color;

void main(void)
{
    gl_FragColor = vec4(color, 1);
}

#endif // _FRAGMENT_