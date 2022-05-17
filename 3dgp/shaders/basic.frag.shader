#version 330

// View Matrix
uniform mat4 matrixView;

// Materials
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;

uniform sampler2D texture0;
uniform sampler2D textureNormal;
//uniform sampler2D textureAO;

//uniform vec3 fogColour;

// Input Variables (received from Vertex Shader)
in vec4 color;
in vec4 position;
vec3 normal;
in vec2 texCoord0;

in mat3 matrixTangent; //Normal Mapping

//in float fogFactor;

// Output Variable (sent down through the Pipeline)
out vec4 outColor;

uniform int Text = 0;


struct DIRECTIONAL
{
	vec3 direction;
	vec3 diffuse;
};
uniform DIRECTIONAL lightDir;

struct RIM
{
	vec3 direction;
	vec3 diffuse;
};
uniform RIM rimlight;

//Point
struct POINT
{
	vec3 position;
	vec3 diffuse;
	vec3 specular;
};
uniform POINT lightPoint1;

//Spot Light
struct SPOT
{
	vec3 position;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
	float cutoff;
	float attenuation;
};
uniform SPOT spotLight1;

vec4 DirectionalLight(DIRECTIONAL light)
{
	// Calculate Directional Light
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L = normalize(mat3(matrixView) * light.direction);
	float NdotL = dot(normal, L);

	vec3 V = normalize(-position.xyz);
	float rim = 1 - dot(V, -L);
	rim = smoothstep(0.6, 1.0, rim);
	//float zero = 0.0f;
	vec4 rimcolor = vec4(light.diffuse.x * rim, light.diffuse.y * rim, light.diffuse.z * rim,0);

	if (NdotL > 0)
		color += vec4(materialDiffuse * light.diffuse, 1) * NdotL;
	//color += rimcolor;

	return color;
}

vec4 RimLight(RIM light)
{
	// Calculate Directional Light
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L = normalize(mat3(matrixView) * light.direction);
	float NdotL = dot(normal, L);

	vec3 V = normalize(-position.xyz);
	float rim = 1 - max(dot(V, normal),0);
	rim = smoothstep(0.6, 1.0, rim);
	vec4 rimcolor = vec4(light.diffuse.x * rim, light.diffuse.y * rim, light.diffuse.z * rim, 0);

	
	color += rimcolor;

	return color;
}

vec4 PointLight(POINT light)
{
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L = (normalize((matrixView * vec4(light.position, 1)) - position)).xyz;
	float NdotL = dot(normal, L);
	if (NdotL > 0)
		color += vec4(materialDiffuse * light.diffuse, 1) * NdotL;

	vec3 V = normalize(-position.xyz);
	vec3 R = reflect(-L, normal);
	float RdotV = dot(R, V);
	if (NdotL > 0 && RdotV > 0)
		color += vec4(materialSpecular * light.specular * pow(RdotV, shininess), 1);

	return color;

}

vec4 SpotLight(SPOT light)
{
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L = (normalize((matrixView * vec4(light.position, 1)) - position)).xyz;
	float NdotL = dot(normal, L);
	if (NdotL > 0)
		color += vec4(materialDiffuse * light.diffuse, 1) * NdotL;

	vec3 V = normalize(-position.xyz);
	vec3 R = reflect(-L, normal);
	float RdotV = dot(R, V);
	if (NdotL > 0 && RdotV > 0)
		color += vec4(materialSpecular * light.specular * pow(RdotV, shininess), 1);

	vec3 D = normalize(mat3(matrixView) * light.direction);
	float s = dot(-L, D);
	float a = acos(s);

	float spotFactor;

	if (a <= light.cutoff) spotFactor = pow(s, light.attenuation);
	else spotFactor = 0;

	return spotFactor * color;

}


void main(void) 
{
	normal = 2.0 * texture(textureNormal, texCoord0).xyz - vec3(1.0, 1.0, 1.0);
	normal = normalize(matrixTangent * normal);

	/*vec3 V = normalize(-position.xyz);
	float rim = 1 - max(dot(V, normal),0);
	rim = smoothstep(0.6, 1.0, rim);*/


	outColor = color;
	outColor += DirectionalLight(lightDir);
	outColor += RimLight(rimlight);
	outColor += PointLight(lightPoint1);
	//outColor += SpotLight(spotLight1);
	outColor *= texture(texture0, texCoord0.st);
	//outColor = mix(vec4(fogColour, 1), outColor, fogFactor);

	// Debug Text - Do Not Change!
	if (Text == 1) outColor = vec4(1, 0.8, 0, 1);
}
