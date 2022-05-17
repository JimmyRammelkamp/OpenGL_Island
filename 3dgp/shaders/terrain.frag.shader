#version 330

// Water-related
uniform vec3 waterColor;

uniform vec3 fogColour;

uniform sampler2D textureBed;
uniform sampler2D textureShore;


in float fogFactor;

// Input: Water Related
in float waterDepth;		// water depth (positive for underwater, negative for the shore)

// Input Variables (received from Vertex Shader)
in vec4 color;
in vec4 position;
in vec3 normal;
in vec2 texCoord0;

// Output Variable (sent down through the Pipeline)
out vec4 outColor;

void main(void) 
{
	outColor = color;

	// shoreline multitexturing
	float isAboveWater = clamp(-waterDepth, 0, 1); 
	outColor *= mix(texture(textureBed, texCoord0), texture(textureShore, texCoord0), isAboveWater);

	outColor = mix(vec4(fogColour, 1), outColor, fogFactor);
}
