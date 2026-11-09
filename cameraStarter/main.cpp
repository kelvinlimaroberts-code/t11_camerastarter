#include <windows.h>
#include <string>
#include <cassert>
#include <d3d11.h>
#include <vector>

#include "WindowUtils.h"
#include "D3D.h"
#include "Sprite.h"
#include "ShaderTypes.h"
#include "FX.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


ID3D11InputLayout* gInputLayout;				//vertex description
ID3D11Buffer* gBoxVB;							//quad vertex buffer
ID3D11Buffer* gBoxIB;							//quad index buffer
ID3D11VertexShader* pVertexShader = nullptr;	//d3d vertex shader
ID3D11PixelShader* pPixelShader = nullptr;		//d3d pixel shader

//the three matrices all 3D apps need
Matrix gWorld;
Matrix gView;
Matrix gProj;


GfxParamsPerObj gGfxData;				//a structure containing data the gpu needs to render something
ID3D11Buffer* gpGfxDataConstsBuffer;	//a d3d object to copy the above data into, ends up in a constant register

Vector3 gCamPos = { 0, 0, -10 };		//where is the camera in the world?
float gFOV = 0.25f*PI;					//what field of view angle do we want
//
//my global variabke to mess around
float radiusOfPath = 4.f;

struct Quad_t
{
	Quad_t(float x, float y, float angl, float speeed, float siize);
	~Quad_t();

	float Xpos = 0.f;
	float Ypos = 0.f;
	float angle = 0.f;
	const float speed = 1.f;
	float size = 1.f;
};

Quad_t::Quad_t(float x, float y, float angl, float speeed, float siize) : speed(speeed)
{
	Xpos = x;
	Ypos = y;
	angle = angl;
	size = siize;
}


Quad_t::~Quad_t()
{
}


Quad_t quadA(0, 0, 0, 1, 1);
Quad_t quadB(0, 0, 180, 1, 1);
Quad_t quadC(0, 0, 0, 1, 1);




float cameraSpeed = 5.f;

//build a quad
void BuildGeometryBuffers()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();

	// Create vertex buffer for a quad (two triangle square)
	VertexPosColour vertices[] =
	{
		{ Vector3(-0.5f, -0.5f, 0.f), Colors::White   },
		{ Vector3(-0.5f, +0.5f, 0.f), Colors::Black },
		{ Vector3(+0.5f, +0.5f, 0.f), Colors::Red },
		{ Vector3(+0.5f, -0.5f, 0.f), Colors::Green }
	};

	CreateVertexBuffer(d3d.GetDevice(),sizeof(VertexPosColour) * 4, vertices, gBoxVB);


	// Create the index buffer

	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3
	};

	CreateIndexBuffer(d3d.GetDevice(), sizeof(UINT) * 6, indices, gBoxIB);
}

//copy the data the gpu needs across to render something
void UpdateConstsPerObj()
{
	gGfxData.wvp = gWorld * gView * gProj;
	WinUtil::Get().GetD3D().GetDeviceCtx().UpdateSubresource(gpGfxDataConstsBuffer, 0, nullptr, &gGfxData, 0, 0);
}

//initialise the shaders and prepare all d3d objects
bool BuildFX()
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	CheckShaderModel5Supported(d3d.GetDevice());

	// Create the constant buffers for the variables defined in the vertex shader.
	CreateConstantBuffer(d3d.GetDevice(),sizeof(GfxParamsPerObj), &gpGfxDataConstsBuffer);

	//load in a pre-compiled vertex shader
	char* pBuff = nullptr;
	unsigned int bytes = 0;
	pBuff = ReadAndAllocate("../bin/data/SimpleVS.cso", bytes);
	CreateVertexShader(d3d.GetDevice(), pBuff, bytes, pVertexShader);
	//create a link between our data and the vertex shader
	CreateInputLayout(d3d.GetDevice(), VertexPosColour::sVertexDesc, 2, pBuff, bytes, &gInputLayout);
	delete[] pBuff;

	//load in a pre-compiled pixel shader	
	pBuff = ReadAndAllocate("../bin/data/SimplePS.cso", bytes);
	CreatePixelShader(d3d.GetDevice(), pBuff, bytes, pPixelShader);
	delete[] pBuff;


	return true;

}

//handy function to get a random number between min+max
float GetRandom(float min, float max)
{
	float alpha = (float)rand() / RAND_MAX;
	alpha *= max - min;
	alpha += min;
	return alpha;
}

//called once to setup all d3d objects and initialse the matrices
void InitGame()
{
	BuildGeometryBuffers();
	BuildFX();

	gWorld = Matrix::Identity;
	gView = Matrix::Identity;
	gProj = Matrix::Identity;

	srand(0);	//seed the random number generator
}

void ReleaseGame()
{
	ReleaseCOM(pVertexShader);
	ReleaseCOM(pPixelShader);
	ReleaseCOM(gpGfxDataConstsBuffer);
	ReleaseCOM(gBoxVB);
	ReleaseCOM(gBoxIB);
	ReleaseCOM(gInputLayout);
}

//for now just update the matrices
void Update(float dTime)
{
	CreateProjectionMatrix(gProj, gFOV, WinUtil::Get().GetAspectRatio(), 1, 1000.f);
	CreateViewMatrix(gView, gCamPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
}

void Render(float dTime)
{
	//wipe the back buffer ready for rendering
	MyD3D& d3d = WinUtil::Get().GetD3D();
	d3d.BeginRender(Colours::Blue);

	//setup for rendering and send the quad
	d3d.InitInputAssembler(gInputLayout, gBoxVB, sizeof(VertexPosColour), gBoxIB);
	d3d.GetDeviceCtx().VSSetShader(pVertexShader, nullptr, 0);
	d3d.GetDeviceCtx().PSSetShader(pPixelShader, nullptr, 0);
	
	
	quadA.angle += quadA.speed * dTime;
	quadA.Xpos = radiusOfPath * cos(quadA.angle);
	quadA.Ypos = radiusOfPath * sin(quadA.angle);

	Matrix trans = Matrix::CreateTranslation(quadA.Xpos, quadA.Ypos, 0);             //trying to move the quad??
	gWorld = trans;
	UpdateConstsPerObj();
	d3d.GetDeviceCtx().VSSetConstantBuffers(0, 1, &gpGfxDataConstsBuffer);
	d3d.GetDeviceCtx().DrawIndexed(6, 0, 0);

	//tell the gpu we've finished
	d3d.EndRender();
}

//if the resolution changes rebuild the projection matrix
void OnResize(int screenWidth, int screenHeight, MyD3D& d3d)
{
	CreateProjectionMatrix(gProj, gFOV, WinUtil::Get().GetAspectRatio(), 1, 1000.f);
	d3d.OnResize_Default(screenWidth, screenHeight);
}

//check for eascape to quit
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//do something game specific here
	switch (msg)
	{
		// Respond to a keyboard event.
	case WM_CHAR:
		switch (wParam)
		{
		case 27:
			PostQuitMessage(0);
			return 0;
			break;
		case 81: //q
			gCamPos.y += cameraSpeed;
			break;
		case 65: //A
			gCamPos.y -= cameraSpeed;
			break;
		case 44: //d
			gCamPos.x -= cameraSpeed;
			break;
		case 46: //f
			gCamPos.x += cameraSpeed;
			break;
		case 87: //w
			gCamPos.z += cameraSpeed;
			break;
		case 83: //s
			gCamPos.z -= cameraSpeed;
		}  
	}

	//default message handling (resize window, full screen, etc)
	return WinUtil::Get().DefaultMssgHandler(hwnd, msg, wParam, lParam);
}

//main entry point for the game
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	//setup both windows and d3d
	int w(512), h(256);
	//int defaults[] = { 640,480, 800,600, 1024,768, 1280,1024 };
		//WinUtil::ChooseRes(w, h, defaults, 4);
	if (!WinUtil::Get().InitMainWindow(w, h, hInstance, "Fezzy", MainWndProc, true))
		assert(false);
	MyD3D d3d;
	if (!d3d.InitDirect3D(OnResize))
		assert(false);
	WinUtil::Get().SetD3D(d3d);
	d3d.GetCache().SetAssetPath("data/");	//tell the cache where to look for textures

	InitGame();

	bool canUpdateRender;
	float dTime = 0;
	while (WinUtil::Get().BeginLoop(canUpdateRender))
	{
		if (canUpdateRender && dTime>0)
		{
			Update(dTime);
			Render(dTime);
		}
		dTime = WinUtil::Get().EndLoop(canUpdateRender);
	}

	d3d.ReleaseD3D(true);	
	return 0;
}

