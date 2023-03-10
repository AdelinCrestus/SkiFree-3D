#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float timeX;
uniform float timeY;
uniform int quad;

// Output
out vec2 texcoord;
out vec3 world_position;
out vec3 world_normal;

void main()
{
    // TODO(student): Pass v_texture_coord as output to fragment shader
    
    texcoord = v_texture_coord;
    world_position = (Model* vec4(v_position, 1)).xyz;
    world_normal = (Model * vec4(v_normal, 1)).xyz;
        
        if(quad == 1)
        {
        texcoord = vec2(texcoord.x - timeX* 0.1 , texcoord.y - timeY*0.1); // 0.1 este factorul de scalare pe axele Ox si Oz din lume pt om
        }
    
    
    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
