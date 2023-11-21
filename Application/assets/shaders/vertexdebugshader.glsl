// Vertex shader
#version 420

//uniform mat4 MVP;
uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matModel;
uniform mat4 matModel_IT;

//uniform vec3 modelScale;
//uniform vec3 modelOffset;

in vec4 vCol;
in vec4 vPos;

out vec4 colour;

void main()
{
	mat4 matMVP = matProjection * matView * matModel;
	gl_Position = matMVP * vec4(vPos.xyz, 1.0);

	colour = vCol;
}
