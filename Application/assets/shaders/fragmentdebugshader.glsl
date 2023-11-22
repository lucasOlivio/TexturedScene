// Fragment shader
#version 420

in vec4 colour;

uniform bool useDebugColor;
uniform vec4 debugColour;

out vec4 outputColour;

void main()
{
	outputColour = colour;

	if (useDebugColor)
	{
		outputColour = debugColour;
	}
}