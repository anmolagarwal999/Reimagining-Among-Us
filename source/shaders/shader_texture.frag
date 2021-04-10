#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragColor;

// output data
out vec4 color;

// texture samplers
uniform sampler2D texture1;

void main()
{
    // Output color = color specified in the vertex shader,
    // interpolated between all 3 surrounding vertices of the triangle
    //color = fragColor;
    
    //color=texture(texture1, vec2( fragColor.x,  fragColor.y));

    vec4 texColor = texture(texture1, vec2( fragColor.x,  fragColor.y));
    if(texColor.a < 0.1)
        discard;
    color = texColor;
}
