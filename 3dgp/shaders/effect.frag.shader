#version 330

// Input Variables (received from Vertex Shader)
in vec2 texCoord0;

// Uniform: The Texture
uniform sampler2D texture0;
uniform vec2 resolution = vec2(800, 600);

// Output Variable (sent down through the Pipeline)
out vec4 outColor;

void main(void)
{
	//outColor = vec4(1, 1, 1, 2) - texture(texture0, texCoord0);
	//outColor = texture(texture0, texCoord0);
	//vec4 tex = texture(texture0, texCoord0);
	//outColor = vec4((vec3(1, 1, 1) - tex.xyz), tex.w);
	
	const int SIZE = 5;

	vec3 v = vec3(0, 0, 0);

	int n = 0;
	for (int k = -SIZE; k <= SIZE; k++)
		for (int j = -SIZE; j <= SIZE; j++)
		{
			v += texture(texture0, texCoord0 + vec2(k, j) / resolution).rgb;
			n++;
		}

	outColor = vec4(v / n, 1);

}