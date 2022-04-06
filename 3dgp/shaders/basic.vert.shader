#version 330

// Uniforms: Transformation Matrices
uniform mat4 matrixProjection;
uniform mat4 matrixView;
uniform mat4 matrixModelView;

// Uniforms: Material Colours
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;

// Bone Transforms
#define MAX_BONES 100
uniform mat4 bones[MAX_BONES];

//uniform float fogDensity;

// Uniforms: Water Related
uniform float waterLevel;	// water level (in absolute units)

in vec3 aVertex;	
in vec3 aNormal;
in vec2 aTexCoord;
in vec3 aTangent;
in vec3 aBiTangent;

in ivec4 aBoneId;		// Bone Ids
in  vec4 aBoneWeight;	// Bone Weights


out vec4 color;
out vec4 position;
out vec3 normal;
out vec2 texCoord0;
out mat3 matrixTangent; //Normal Mapping

// Output: Water Related
//out float waterDepth;	// water depth (positive for underwater, negative for the shore)

//out float fogFactor;

// Light declarations
struct AMBIENT
{	
	vec3 color;
};
uniform AMBIENT lightAmbient;

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
uniform POINT lightPoint1, lightPoint2;

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

vec4 AmbientLight(AMBIENT light)
{
	// Calculate Ambient Light
	return vec4(materialAmbient * light.color, 1);
}

//vec4 DirectionalLight(DIRECTIONAL light)
//{
//	// Calculate Directional Light
//	vec4 color = vec4(0, 0, 0, 0);
//	vec3 L = normalize(mat3(matrixView) * light.direction);
//	float NdotL = dot(normal, L);
//	if (NdotL > 0)
//		color += vec4(materialDiffuse * light.diffuse, 1) * NdotL;
//	return color;
//}

void main(void) 
{
	
	mat4 matrixBone;
	if (aBoneWeight[0] == 0.0)
		matrixBone = mat4(1);
	else
		matrixBone = (bones[aBoneId[0]] * aBoneWeight[0] +
					  bones[aBoneId[1]] * aBoneWeight[1] +
					  bones[aBoneId[2]] * aBoneWeight[2] +
					  bones[aBoneId[3]] * aBoneWeight[3]);

	
	// calculate position
	position = matrixModelView * matrixBone * vec4(aVertex, 1.0);
	gl_Position = matrixProjection * position;

	// calculate normal
	normal = normalize(mat3(matrixModelView) * mat3(matrixBone) * aNormal);

	// calculate texture coordinate
	texCoord0 = aTexCoord;

	// calculate tangent local system transformation
	vec3 tangent = normalize(mat3(matrixModelView) * aTangent);
	vec3 biTangent = normalize(mat3(matrixModelView) * aBiTangent);
	matrixTangent = mat3(tangent, biTangent, normal);


	// calculate depth of water
	//waterDepth = waterLevel - aVertex.y;

	// calculate the observer's altitude above the observed vertex
	//float eyeAlt = dot(-position.xyz, mat3(matrixModelView) * vec3(0, 1, 0));

	// calculate fogfactor
	//fogFactor = exp2(-fogDensity * length(position) * (max(waterDepth,0) / max(eyeAlt,0.1f)) );

	// calculate light
	color = vec4(0, 0, 0, 1);
	color += AmbientLight(lightAmbient);
	//color += DirectionalLight(lightDir);
}
