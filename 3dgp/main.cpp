#include <iostream>
#include "GL/glew.h"
#include "GL/3dgl.h"
#include "GL/glut.h"
#include "GL/freeglut_ext.h"

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#pragma comment (lib, "glew32.lib")

using namespace std;
using namespace _3dgl;
using namespace glm;

// 3D Models
C3dglSkyBox skybox;

C3dglTerrain terrain, water;
C3dglModel streetLamp;
C3dglModel character;



C3dglProgram ProgramBasic;
C3dglProgram ProgramWater;
C3dglProgram ProgramTerrain;
C3dglProgram ProgramEffect;

// Textures 
GLuint idTexSand;
GLuint idTexGrass;
GLuint idTexNone;
GLuint idTexScreen;
GLuint idTexWood;
GLuint idTexNormal;

GLuint WImage = 800, HImage = 600;
GLuint idFBO;


unsigned bufQuad = 0;

// Water specific variables
float waterLevel = 4.6f;

//fog variables 

// camera position (for first person type camera navigation)
mat4 matrixView;			// The View Matrix
float angleTilt = 15.f;		// Tilt Angle
vec3 cam(0);				// Camera movement values

//float PrevT;
//vec3 characterpos;

bool init()
{
	//PrevT = 0;
	//characterpos = vec3(1.0f, 5.0f, 20.0f);
	// rendering states
	glEnable(GL_DEPTH_TEST);	// depth test is necessary for most 3D scenes
	glEnable(GL_NORMALIZE);		// normalization is needed by AssImp library models
	glShadeModel(GL_SMOOTH);	// smooth shading mode is the default one; try GL_FLAT here!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// this is the default one; try GL_LINE!

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Initialise Shaders
	C3dglShader VertexShader;
	C3dglShader FragmentShader;


	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/basic.vert.shader")) return false;
	if (!VertexShader.Compile()) return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/basic.frag.shader")) return false;
	if (!FragmentShader.Compile()) return false;

	if (!ProgramBasic.Create()) return false;
	if (!ProgramBasic.Attach(VertexShader)) return false;
	if (!ProgramBasic.Attach(FragmentShader)) return false;
	if (!ProgramBasic.Link()) return false;
	if (!ProgramBasic.Use(true)) return false;

	// Initialise Water Shaders

	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/water.vert.shader")) return false;
	if (!VertexShader.Compile()) return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/water.frag.shader")) return false;
	if (!FragmentShader.Compile()) return false;

	if (!ProgramWater.Create()) return false;
	if (!ProgramWater.Attach(VertexShader)) return false;
	if (!ProgramWater.Attach(FragmentShader)) return false;
	if (!ProgramWater.Link()) return false;
	if (!ProgramWater.Use(true)) return false;

	// Initialise Terrain Shaders

	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/terrain.vert.shader")) return false;
	if (!VertexShader.Compile()) return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/terrain.frag.shader")) return false;
	if (!FragmentShader.Compile()) return false;

	if (!ProgramTerrain.Create()) return false;
	if (!ProgramTerrain.Attach(VertexShader)) return false;
	if (!ProgramTerrain.Attach(FragmentShader)) return false;
	if (!ProgramTerrain.Link()) return false;
	if (!ProgramTerrain.Use(true)) return false;

	// Initialise Effect Shaders

	if (!VertexShader.Create(GL_VERTEX_SHADER)) return false;
	if (!VertexShader.LoadFromFile("shaders/effect.vert.shader")) return false;
	if (!VertexShader.Compile()) return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER)) return false;
	if (!FragmentShader.LoadFromFile("shaders/effect.frag.shader")) return false;
	if (!FragmentShader.Compile()) return false;

	if (!ProgramEffect.Create()) return false;
	if (!ProgramEffect.Attach(VertexShader)) return false;
	if (!ProgramEffect.Attach(FragmentShader)) return false;
	if (!ProgramEffect.Link()) return false;
	if (!ProgramEffect.Use(true)) return false;

	ProgramBasic.Use();
	// glut additional setup
	glutSetVertexAttribCoord3(ProgramBasic.GetAttribLocation("aVertex"));
	glutSetVertexAttribNormal(ProgramBasic.GetAttribLocation("aNormal"));

	// Create Quad
	float vertices[] = {
		0.0f, 0.0f, 0.0f,	0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,	1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,	1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,	0.0f, 1.0f
	};


	// Generate the buffer name
	glGenBuffers(1, &bufQuad);
	// Bind the vertex buffer and send data
	glBindBuffer(GL_ARRAY_BUFFER, bufQuad);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// load your 3D models here!
	if (!terrain.loadHeightmap("models\\heightmap.png", 10)) return false;
	if (!water.loadHeightmap("models\\watermap.png", 10)) return false;
	
	if (!streetLamp.load("models\\street lamp - fancy.obj")) return false;
	if (!character.load("models\\Happy Idle.dae")) return false;
	if (!character.loadAnimations()) return false;
	
	if (!skybox.load("models\\TropicalSunnyDay\\TropicalSunnyDayFront1024.jpg",
		"models\\TropicalSunnyDay\\TropicalSunnyDayLeft1024.jpg",
		"models\\TropicalSunnyDay\\TropicalSunnyDayBack1024.jpg",
		"models\\TropicalSunnyDay\\TropicalSunnyDayRight1024.jpg",
		"models\\TropicalSunnyDay\\TropicalSunnyDayUp1024.jpg",
		"models\\TropicalSunnyDay\\TropicalSunnyDayDown1024.jpg")) return false;

	// setup the textures
	C3dglBitmap bm;

	// none (simple-white) texture
	glGenTextures(1, &idTexNone);
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	BYTE bytes[] = { 255, 255, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_BGR, GL_UNSIGNED_BYTE, &bytes);

	// Create screen space texture
	glGenTextures(1, &idTexScreen);
	glBindTexture(GL_TEXTURE_2D, idTexScreen);
	// Texture parameters - to get nice filtering 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// This will allocate an uninitilised texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WImage, HImage, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	bm.Load("models/textures/Ch19_1001_Diffuse.png", GL_RGBA);
	if (!bm.GetBits()) return false;
	glGenTextures(1, &idTexWood);
	glBindTexture(GL_TEXTURE_2D, idTexWood);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.GetBits());

	bm.Load("models/sand.png", GL_RGBA);
	if (!bm.GetBits()) return false;
	glGenTextures(1, &idTexSand);
	glBindTexture(GL_TEXTURE_2D, idTexSand);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.GetBits());

	bm.Load("models/textures/Ch19_1001_Normal.png", GL_RGBA);
	if (!bm.GetBits()) return false;
	glGenTextures(1, &idTexNormal);
	glBindTexture(GL_TEXTURE_2D, idTexNormal);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.GetBits());
	
	bm.Load("models/grass.png", GL_RGBA);
	if (!bm.GetBits()) return false;
	glGenTextures(1, &idTexGrass);
	glBindTexture(GL_TEXTURE_2D, idTexGrass);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bm.GetBits());


	ProgramBasic.SendUniform("texture0", 0);
	//ProgramEffect.SendUniform("texture0", 0);
	ProgramBasic.SendUniform("textureNormal", 1);
	ProgramTerrain.SendUniform("textureBed", 2);
	ProgramTerrain.SendUniform("textureShore", 3);
	ProgramEffect.SendUniform("texture0", 4);

	

	
	// setup lights (for basic and terrain programs only, water does not use these lights):
	ProgramBasic.SendUniform("lightAmbient.color", 0.1, 0.1, 0.1);
	ProgramTerrain.SendUniform("lightAmbient.color", 0.1, 0.1, 0.1);
	ProgramBasic.SendUniform("lightDir.direction", 1.0, 0.5, 1.0);
	ProgramTerrain.SendUniform("lightDir.direction", 1.0, 0.5, 1.0);
	ProgramBasic.SendUniform("lightDir.diffuse", 0.5, 0.5, 0.5);
	ProgramTerrain.SendUniform("lightDir.diffuse", 0.5, 0.5, 0.5);

	//setup rimlighting
	ProgramBasic.SendUniform("rimlight.direction", 0.0, 0.0, 0.0);
	ProgramBasic.SendUniform("rimlight.diffuse", 0.0, 0.0, 0.0);

	// setup materials (for basic and terrain programs only, water does not use these materials):
	ProgramBasic.SendUniform("materialAmbient", 1.0, 1.0, 1.0);		// full power (note: ambient light is extremely dim)
	ProgramTerrain.SendUniform("materialAmbient", 1.0, 1.0, 1.0);		// full power (note: ambient light is extremely dim)
	ProgramBasic.SendUniform("materialDiffuse", 1.0, 1.0, 1.0);
	ProgramTerrain.SendUniform("materialDiffuse", 1.0, 1.0, 1.0);

	// setup the water colours and level
	ProgramWater.SendUniform("waterColor", 0.2f, 0.22f, 0.02f);
	ProgramWater.SendUniform("skyColor", 0.2f, 0.6f, 1.f);
	ProgramTerrain.SendUniform("waterColor", 0.2f, 0.22f, 0.02f);
	ProgramTerrain.SendUniform("waterLevel", waterLevel);
	//ProgramBasic.SendUniform("waterLevel", waterLevel);

	// setup fog
	//ProgramBasic.SendUniform("fogColour", 0.2f, 0.22f, 0.02f);
	//ProgramBasic.SendUniform("fogDensity", 0.3f);
	ProgramTerrain.SendUniform("fogColour", 0.2f, 0.22f, 0.02f);
	ProgramTerrain.SendUniform("fogDensity", 0.3f);

	ProgramTerrain.SendUniform("lightAmbient.on", 1);
	ProgramTerrain.SendUniform("lightDir.on", 1);

	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));
	matrixView *= lookAt(
		vec3(4.0, 0.4, 30.0),
		vec3(4.0, 0.4, 0.0),
		vec3(0.0, 1.0, 0.0));

	// setup the screen background colour
	glClearColor(0.2f, 0.6f, 1.f, 1.0f);   // blue sky colour

	cout << endl;
	cout << "Use:" << endl;
	cout << "  WASD or arrow key to navigate" << endl;
	cout << "  QE or PgUp/Dn to move the camera up and down" << endl;
	cout << "  Drag the mouse to look around" << endl;
	cout << endl;


	// Create a framebuffer object (FBO)
	glGenFramebuffers(1, &idFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, idFBO);

	// Attach a depth buffer
	GLuint depth_rb;
	glGenRenderbuffers(1, &depth_rb);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, WImage, HImage);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rb);

	// attach the texture to FBO colour attachment point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, idTexScreen, 0);

	// switch back to window-system-provided framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);

	return true;
}

void done()
{
}

void onReshape(int w, int h);

void renderScene(mat4& matrixView, float time)
{
	mat4 m;

	

	// Setup the Diffuse Material to: Green Grass
	//ProgramBasic.SendUniform("materialDiffuse", 0.2f, 0.8f, 0.2f);
	//ProgramTerrain.SendUniform("materialDiffuse", 0.2f, 0.8f, 0.2f);

	//Program.SendUniform("materialDiffuse", 0.1f, 0.0f, 2.0f);
	//Program.SendUniform("materialSpecular", 1.0, 1.0, 1.0);
	//Program.SendUniform("shininess", 10.0);

	ProgramBasic.Use();
	glActiveTexture(GL_TEXTURE0);

	// render the skybox

	//Program.SendUniform("materialDiffuse", 0.1f, 0.0f, 2.0f);
	//Program.SendUniform("materialSpecular", 1.0, 1.0, 1.0);

	m = matrixView;
	ProgramBasic.SendUniform("lightAmbient.color", 1.0, 1.0, 1.0);
	ProgramBasic.SendUniform("materialDiffuse", 0.0, 0.0, 0.0);
	skybox.render(m);
	


	//spotlight
	ProgramBasic.SendUniform("spotLight1.position", 1.0f, 6.0f, 20.0f);
	ProgramBasic.SendUniform("spotLight1.diffuse", 1.0, 0.1, 0.1);
	ProgramBasic.SendUniform("spotLight1.specular", 0.6, 0.6, 0.6);
	ProgramBasic.SendUniform("spotLight1.direction", 0.0, -1.0, 0.0);
	ProgramBasic.SendUniform("spotLight1.cutoff", radians(15.0));
	ProgramBasic.SendUniform("spotLight1.attenuation", 20.0f);

	//prepare ambient light for the skybox
	//ProgramBasic.SendUniform("lightAmbient.color", 1.0, 1.0, 1.0);
	//ProgramBasic.SendUniform("materialDiffuse", 0.0, 0.0, 0.0);
	
	
	// revert normal light after skybox
	ProgramBasic.SendUniform("lightAmbient.color", 0.1, 0.1, 0.1);
	ProgramBasic.SendUniform("materialDiffuse", 1.0, 1.0, 1.0);

	ProgramBasic.SendUniform("rimlight.direction", 1.0, 0.5, 1.0);
	ProgramBasic.SendUniform("rimlight.diffuse", 0.5, 0.5, 0.5);
	
	m = matrixView;
	m = translate(m, vec3(0.0f, 3.0f, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.08f, 0.08f, 0.08f));
	streetLamp.render(m);
	glBindTexture(GL_TEXTURE_2D, idTexWood);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, idTexNormal);
	// calculate and send bone transforms
	std::vector<float> transforms;
	character.getAnimData(0, time, transforms);
	ProgramBasic.SendUniformMatrixv("bones", (float*)&transforms[0], transforms.size() / 16);
	//cout << time;
	
	static float PrevT = 0.0f;
	//float delta = time - PrevT;

	//cout << time - PrevT << " ";
	static vec3 characterpos = vec3(1.0f, 5.0f, 20.0f);
	vec3 target = vec3(20.0f, 5.0f, 40.0f);
	vec3 direction = normalize(target - characterpos);
	vec3 transformation = (direction * (time - PrevT)) * 2.0f;
	

	if (length(characterpos - target) < 0.1f)
		characterpos = characterpos;
	else characterpos = characterpos + transformation;
	//cout << (length(characterpos - target)) << " ";
	PrevT = time;
	//cout << normalize(target - characterpos).x << " ";
	m = matrixView;
	m = translate(m, characterpos);
	m = rotate(m, atan2(direction.x,direction.z), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(1.0f, 1.0f, 1.0f));
	character.render(m);
	
	 
	ProgramBasic.SendUniform("rimlight.direction", 0.0, 0.0, 0.0);
	ProgramBasic.SendUniform("rimlight.diffuse", 0.0, 0.0, 0.0);


	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, idTexSand);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, idTexGrass);
	// render the terrain
	ProgramTerrain.Use();
	m = matrixView;
	terrain.render(m);

	


	// Setup the Diffuse Material to: Watery Green
	//ProgramBasic.SendUniform("materialDiffuse", 0.2f, 0.22f, 0.02f);
	//ProgramTerrain.SendUniform("materialDiffuse", 0.2f, 0.22f, 0.02f);
	ProgramWater.SendUniform("materialDiffuse", 0.2f, 0.22f, 0.02f);

	// render the water
	ProgramWater.Use();
	m = matrixView;
	m = translate(m, vec3(0, waterLevel, 0));
	m = scale(m, vec3(0.5f, 1.0f, 0.5f));
	ProgramWater.SendUniform("matrixModelView", m);
	water.render(m);

}

//void onReshape(int w, int h);

void onRender()
{
	// send the animation time to shaders
	ProgramWater.SendUniform("t", glutGet(GLUT_ELAPSED_TIME) / 1000.f);

	// this global variable controls the animation
	float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;

	
	// Pass 1: off-screen rendering
	glBindFramebufferEXT(GL_FRAMEBUFFER, idFBO);


	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup the View Matrix (camera)
	mat4 m = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));// switch tilt off
	m = translate(m, cam);												// animate camera motion (controlled by WASD keys)
	m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));			// switch tilt on
	matrixView = m * matrixView;

	// move the camera up following the profile of terrain (Y coordinate of the terrain)
	float terrainY = -std::max(terrain.getInterpolatedHeight(inverse(matrixView)[3][0], inverse(matrixView)[3][2]), waterLevel);
	matrixView = translate(matrixView, vec3(0, terrainY, 0));

	// setup View Matrix
	ProgramBasic.SendUniform("matrixView", matrixView);
	ProgramWater.SendUniform("matrixView", matrixView);
	ProgramTerrain.SendUniform("matrixView", matrixView);

	// render the scene objects
	renderScene(matrixView, time);

	// the camera must be moved down by terrainY to avoid unwanted effects
	matrixView = translate(matrixView, vec3(0, -terrainY, 0));

	
	// Pass 2: on-screen rendering
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
	
	

	// setup ortographic projection
	ProgramEffect.SendUniform("matrixProjection", ortho(0, 1, 0, 1, -1, 1));
	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, idTexScreen);

	// setup identity matrix as the model-view
	ProgramEffect.SendUniform("matrixModelView", mat4(1));

	GLuint attribVertex = ProgramEffect.GetAttribLocation("aVertex");
	GLuint attribTextCoord = ProgramEffect.GetAttribLocation("aTexCoord");
	glEnableVertexAttribArray(attribVertex);
	glEnableVertexAttribArray(attribTextCoord);
	glBindBuffer(GL_ARRAY_BUFFER, bufQuad);
	glVertexAttribPointer(attribVertex, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glVertexAttribPointer(attribTextCoord, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableVertexAttribArray(attribVertex);
	glDisableVertexAttribArray(attribTextCoord);

	// essential for double-buffering technique
	glutSwapBuffers();

	// proceed the animation
	glutPostRedisplay();
}

// called before window opened or resized - to setup the Projection Matrix
void onReshape(int w, int h)
{
	float ratio = w * 1.0f / h;      // we hope that h is not zero
	glViewport(0, 0, w, h);
	mat4 m = perspective(radians(60.f), ratio, 0.02f, 1000.f);
	ProgramBasic.SendUniform("matrixProjection", m);
	ProgramWater.SendUniform("matrixProjection", m);
	ProgramTerrain.SendUniform("matrixProjection", m);
	//ProgramEffect.SendUniform("matrixProjection", m);
}

// Handle WASDQE keys
void onKeyDown(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w': cam.z = std::max(cam.z * 1.05f, 0.01f); break;
	case 's': cam.z = std::min(cam.z * 1.05f, -0.01f); break;
	case 'a': cam.x = std::max(cam.x * 1.05f, 0.01f); break;
	case 'd': cam.x = std::min(cam.x * 1.05f, -0.01f); break;
	case 'e': cam.y = std::max(cam.y * 1.05f, 0.01f); break;
	case 'q': cam.y = std::min(cam.y * 1.05f, -0.01f); break;
	}
	// speed limit
	cam.x = std::max(-0.15f, std::min(0.15f, cam.x));
	cam.y = std::max(-0.15f, std::min(0.15f, cam.y));
	cam.z = std::max(-0.15f, std::min(0.15f, cam.z));
}

// Handle WASDQE keys (key up)
void onKeyUp(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	case 's': cam.z = 0; break;
	case 'a':
	case 'd': cam.x = 0; break;
	case 'q':
	case 'e': cam.y = 0; break;
	}
}

// Handle arrow keys and Alt+F4
void onSpecDown(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F4:		if ((glutGetModifiers() & GLUT_ACTIVE_ALT) != 0) exit(0); break;
	case GLUT_KEY_UP:		onKeyDown('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyDown('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyDown('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyDown('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyDown('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyDown('e', x, y); break;
	case GLUT_KEY_F11:		glutFullScreenToggle();
	}
}

// Handle arrow keys (key up)
void onSpecUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:		onKeyUp('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyUp('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyUp('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyUp('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyUp('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyUp('e', x, y); break;
	}
}

// Handle mouse click
bool bJustClicked = false;
void onMouse(int button, int state, int x, int y)
{
	bJustClicked = (state == GLUT_DOWN);
	glutSetCursor(bJustClicked ? GLUT_CURSOR_CROSSHAIR : GLUT_CURSOR_INHERIT);
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
}

// handle mouse move
void onMotion(int x, int y)
{
	if (bJustClicked)
		bJustClicked = false;
	else
	{
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// find delta (change to) pan & tilt
		float deltaPan = 0.25f * (x - glutGet(GLUT_WINDOW_WIDTH) / 2);
		float deltaTilt = 0.25f * (y - glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// View = Tilt * DeltaPan * Tilt^-1 * DeltaTilt * View;
		angleTilt += deltaTilt;
		mat4 m = mat4(1.f);
		m = rotate(m, radians(angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaPan), vec3(0.f, 1.f, 0.f));
		m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaTilt), vec3(1.f, 0.f, 0.f));
		matrixView = m * matrixView;
	}
}

int main(int argc, char **argv)
{
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("CI5520 3D Graphics Programming");

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cerr << "GLEW Error: " << glewGetErrorString(err) << endl;
		return 0;
	}
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;

	// register callbacks
	glutDisplayFunc(onRender);
	glutReshapeFunc(onReshape);
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);

	cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "Version: " << glGetString(GL_VERSION) << endl;

	// init light and everything – not a GLUT or callback function!
	if (!init())
	{
		cerr << "Application failed to initialise" << endl;
		return 0;
	}

	// enter GLUT event processing cycle
	glutMainLoop();

	done();

	return 1;
}

