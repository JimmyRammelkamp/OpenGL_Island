#version 330

// Input Variables (received from Vertex Shader)
in vec2 texCoord0;

// Uniform: The Texture
uniform sampler2D texture0;
uniform vec2 resolution = vec2(800, 600);

// Vignette parameters
const float RADIUS = 0.75;
const float SOFTNESS = 0.45;


const float edgeThreshold = 0.25;

// Colour definitions
const vec3 lum = vec3(0.299, 0.587, 0.114);	// B&W filter
const vec3 sepia = vec3(1.0, 1.0, 1.2);


uniform bool blurPostProc;
uniform bool isAboveWater;

// Output Variable (sent down through the Pipeline)
out vec4 outColor;

//void main(void)
//{
//	//outColor = vec4(1, 1, 1, 2) - texture(texture0, texCoord0);
//	
//	//vec4 tex = texture(texture0, texCoord0);
//	//outColor = vec4((vec3(1, 1, 1) - tex.xyz), tex.w);
//
//	//outColor = texture(texture0, texCoord0);
//	
//	
//
//	
//
//	float s00 = dot(lum, texture(texture0, texCoord0 + vec2(-1, 1) / resolution).rgb);
//	float s01 = dot(lum, texture(texture0, texCoord0 + vec2(0, 1) / resolution).rgb);
//	float s02 = dot(lum, texture(texture0, texCoord0 + vec2(1, 1) / resolution).rgb);
//	float s10 = dot(lum, texture(texture0, texCoord0 + vec2(-1, 0) / resolution).rgb);
//	float s12 = dot(lum, texture(texture0, texCoord0 + vec2(1, 0) / resolution).rgb);
//	float s20 = dot(lum, texture(texture0, texCoord0 + vec2(-1, -1) / resolution).rgb);
//	float s21 = dot(lum, texture(texture0, texCoord0 + vec2(0, -1) / resolution).rgb);
//	float s22 = dot(lum, texture(texture0, texCoord0 + vec2(1, -1) / resolution).rgb);
//
//	float sx = s00 + 2 * s10 + s20 - (s02 + 2 * s12 + s22);
//	float sy = s00 + 2 * s01 + s02 - (s20 + 2 * s21 + s22);
//
//	float s = sx * sx + sy * sy;
//
//	if (s > edgeThreshold)
//		outColor = vec4(1.0);
//	else
//		outColor = vec4(0.0, 0.0, 0.0, 1.0);
//
//	const int SIZE = 5;
//
//	vec3 v = vec3(0, 0, 0);
//
//	int n = 0;
//	for (int k = -SIZE; k <= SIZE; k++)
//		for (int j = -SIZE; j <= SIZE; j++)
//		{
//			v += texture(texture0, texCoord0 + vec2(k, j) / resolution).rgb;
//			n++;
//		}
//
//
//w
//	if (blurPostProc) outColor = vec4(v / n, 1);
//
//}

//layout(binding = 0) uniform sampler2D texture0;

vec2 u_texelStep = vec2( 1.0f / 1280.0f, 1.0f / 720.0f);
uniform int u_showEdges = 0;
uniform int u_fxaaOn = 1;

float u_lumaThreshold = 0.5f;
float u_mulReduce = 8.0f;
float u_minReduce = 128.0f;
float u_maxSpan = 8.0f;

//in vec2 texCoord0;

//out vec4 outColor;

// see FXAA
// http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
// http://iryoku.com/aacourse/downloads/09-FXAA-3.11-in-15-Slides.pdf
// http://horde3d.org/wiki/index.php5?title=Shading_Technique_-_FXAA

//void main(void)
//{
//	vec3 rgbM = texture(texture0, texCoord0).rgb;
//
//	// Possibility to toggle FXAA on and off.
//	if (u_fxaaOn == 0)
//	{
//		outColor = vec4(rgbM, 1.0);
//
//		return;
//	}
//
//	// Sampling neighbour texels. Offsets are adapted to OpenGL texture coordinates. 
//	vec3 rgbNW = textureOffset(texture0, texCoord0, ivec2(-1, 1)).rgb;
//	vec3 rgbNE = textureOffset(texture0, texCoord0, ivec2(1, 1)).rgb;
//	vec3 rgbSW = textureOffset(texture0, texCoord0, ivec2(-1, -1)).rgb;
//	vec3 rgbSE = textureOffset(texture0, texCoord0, ivec2(1, -1)).rgb;
//
//	// see http://en.wikipedia.org/wiki/Grayscale
//	const vec3 toLuma = vec3(0.299, 0.587, 0.114);
//	//const vec3 toLuma = vec3(0.2126, 0.7152, 0.0722);
//
//	// Convert from RGB to luma.
//	float lumaNW = dot(rgbNW, toLuma);
//	float lumaNE = dot(rgbNE, toLuma);
//	float lumaSW = dot(rgbSW, toLuma);
//	float lumaSE = dot(rgbSE, toLuma);
//	float lumaM = dot(rgbM, toLuma);
//
//	// Gather minimum and maximum luma.
//	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
//	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
//
//	// If contrast is lower than a maximum threshold ...
//	if (lumaMax - lumaMin <= lumaMax * u_lumaThreshold)
//	{
//		// ... do no AA and return.
//		outColor = vec4(rgbM, 1.0);
//
//		return;
//	}
//
//	// Sampling is done along the gradient.
//	vec2 samplingDirection;
//	samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
//	samplingDirection.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));
//
//	// Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
//	// This results, that brighter areas are less blurred/more sharper than dark areas.  
//	float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * u_mulReduce, u_minReduce);
//
//	// Factor for norming the sampling direction plus adding the brightness influence. 
//	float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);
//
//	// Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size. 
//	samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-u_maxSpan), vec2(u_maxSpan)) * u_texelStep;
//
//	// Inner samples on the tab.
//	vec3 rgbSampleNeg = texture(texture0, texCoord0 + samplingDirection * (1.0 / 3.0 - 0.5)).rgb;
//	vec3 rgbSamplePos = texture(texture0, texCoord0 + samplingDirection * (2.0 / 3.0 - 0.5)).rgb;
//
//	vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;
//
//	// Outer samples on the tab.
//	vec3 rgbSampleNegOuter = texture(texture0, texCoord0 + samplingDirection * (0.0 / 3.0 - 0.5)).rgb;
//	vec3 rgbSamplePosOuter = texture(texture0, texCoord0 + samplingDirection * (3.0 / 3.0 - 0.5)).rgb;
//
//	vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;
//
//	// Calculate luma for checking against the minimum and maximum value.
//	float lumaFourTab = dot(rgbFourTab, toLuma);
//
//
//	// Are outer samples of the tab beyond the edge ... 
//	if (lumaFourTab < lumaMin || lumaFourTab > lumaMax)
//	{
//		// ... yes, so use only two samples.
//		outColor = vec4(rgbTwoTab, 1.0);
//	}
//	else
//	{
//		// ... no, so use four samples. 
//		outColor = vec4(rgbFourTab, 1.0);
//	}
//
//	// Show edges for debug purposes.	
//	if (u_showEdges != 0)
//	{
//		outColor.r = 1;
//	}
//}

//https://gamedevelopment.tutsplus.com/tutorials/using-displacement-shaders-to-create-an-underwater-effect--cms-2719

uniform float GlobalTime;

void main(void)
{
	
	vec2 uv = texCoord0.xy;
	

	
	
	if(isAboveWater == true)
		outColor = texture(texture0, uv);
	else
	{
		

		//waterdistortion by shifting coordinates
		
		uv.y += cos(uv.x * 25. + GlobalTime) * 0.01;
		uv.x += cos(uv.y * 25. + GlobalTime) * 0.01;
		//outColor = texture2D(texture0, uv);

		const int SIZE = 5;

		vec3 v = vec3(0, 0, 0);

		int n = 0;
		for (int k = -SIZE; k <= SIZE; k++)
			for (int j = -SIZE; j <= SIZE; j++)
			{
				v += texture(texture0, uv + vec2(k, j) / resolution).rgb;
				n++;
			}
		outColor = vec4(v / n, 1);

		// Vignette

	// Find centre position
		vec2 centre = (gl_FragCoord.xy / resolution.xy) - vec2(0.5);

		// Distance from the centre (between 0 and 1)
		float dist = length(centre);

		// Hermite interpolation to create smooth vignette
		dist = smoothstep(RADIUS, RADIUS - SOFTNESS, dist);

		// mix in the vignette
		outColor.rgb = mix(outColor.rgb, outColor.rgb * dist, 0.5);

		// Sepia

		// Find gray scale value using NTSC conversion weights
		float gray = dot(outColor.rgb, lum);

		// mix-in the sepia effect
		outColor.rgb = mix(outColor.rgb, vec3(gray) * sepia, 0.5);
	}

	float s00 = dot(lum, texture(texture0, uv + vec2(-1, 1) / resolution).rgb);
	float s01 = dot(lum, texture(texture0, uv + vec2(0, 1) / resolution).rgb);
	float s02 = dot(lum, texture(texture0, uv + vec2(1, 1) / resolution).rgb);
	float s10 = dot(lum, texture(texture0, uv + vec2(-1, 0) / resolution).rgb);
	float s12 = dot(lum, texture(texture0, uv + vec2(1, 0) / resolution).rgb);
	float s20 = dot(lum, texture(texture0, uv + vec2(-1, -1) / resolution).rgb);
	float s21 = dot(lum, texture(texture0, uv + vec2(0, -1) / resolution).rgb);
	float s22 = dot(lum, texture(texture0, uv + vec2(1, -1) / resolution).rgb);

	float sx = s00 + 2 * s10 + s20 - (s02 + 2 * s12 + s22);
	float sy = s00 + 2 * s01 + s02 - (s20 + 2 * s21 + s22);

	float s = sx * sx + sy * sy;

	if (s > edgeThreshold)
	{
		//outColor = vec4(0.0, 0.0, 0.0, 1.0);
	}

	//outColor = texture(texture0, texCoord0);
}
