#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragColor;

// output data
out vec3 color;

// texture samplers
uniform sampler2D texture1;

void main()
{
    // Output color = color specified in the vertex shader,
    // interpolated between all 3 surrounding vertices of the triangle
    color = fragColor;
    //color=texture(texture1, vec2( fragColor.x,  fragColor.y));
}
