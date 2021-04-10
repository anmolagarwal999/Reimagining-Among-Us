#version 330 core

// Interpolated values from the vertex shaders
in vec3 fragColor;
in vec4 coeffs;

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
    vec4 texColor;
vec4 mult_dim= vec4(0.5f, 0.5f, 0.5f, 1.0);
vec4 mult_bright= vec4(1.0f, 1.0f, 1.0f, 1.0);

    texColor =coeffs*texture(texture1, vec2( fragColor.x,  fragColor.y));


    texColor =mult_dim*texture(texture1, vec2( fragColor.x,  fragColor.y));


    if(texColor.a < 0.1)
        discard;
    color = texColor;
}
