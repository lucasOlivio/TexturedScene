// Vertex shader
#version 420

//uniform mat4 MVP;
uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matModel;	
uniform mat4 matModel_IT;	

// Height texture
uniform vec2 UVOffset;
uniform float heightScale;
uniform bool bUseHeightMapTexture;
uniform sampler2D heightMapTexture;

// Vertex variables
in vec4 vCol;		
in vec4 vPos;	
in vec4 vNormal;
in vec2 vTextureCoords;

// Fragment shader variables
out vec4 colour;
out vec4 vertexWorldPos;	
out vec4 vertexWorldNormal;
out vec2 textureCoords;

void main()
{
	vec4 vertexModelPosition = vPos;

	vec2 UVFinal = vTextureCoords.st + UVOffset.yx;

	// Rotate the normal by the inverse transpose of the model matrix
	// (so that it only is impacted by the rotation, not translation or scale)
	vertexWorldNormal = matModel_IT * vec4(vNormal.xyz, 1.0f);
	vertexWorldNormal.xyz = normalize(vertexWorldNormal.xyz);
	vertexWorldNormal.w = 1.0f;

	if (bUseHeightMapTexture)
	{
		// Greyscale (black and white) heightmap image
		float height = texture(heightMapTexture, UVFinal.st).r;

		// Adjust the height of the y axis from this vertice
		vertexModelPosition += vertexWorldNormal * (height * heightScale);
	}

	mat4 matMVP = matProjection * matView * matModel;
	gl_Position = matMVP * vec4(vertexModelPosition.xyz, 1.0);
	
	vertexWorldPos = matModel * vec4(vertexModelPosition.xyz, 1.0f);
	colour = vCol;
	textureCoords = UVFinal;
}

