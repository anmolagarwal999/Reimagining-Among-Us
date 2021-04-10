#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

//out vec2 TexCoord;
out vec4 TexCoord;
out vec4 coeffs;


uniform vec3 origin_coordinates;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord =vec4(aPos, 1.0);

	  float dx=(aPos.x-origin_coordinates.x);
    float dy=(aPos.y-origin_coordinates.y);

    float dis=dx*dx+dy*dy;
    if(dis<10.0f || (origin_coordinates.z==0.0f))
    {
        coeffs=vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    else
    {
        coeffs=vec4(0.2f, 0.3f, 0.1f, 1.0f);

    }
}

