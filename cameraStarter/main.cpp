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


ID3D11InputLayout* gInputLayout;
ID3D11Buffer* gBoxVB;
ID3D11Buffer* gBoxIB;
ID3D11VertexShader* pVertexShader = nullptr;
ID3D11PixelShader* pPixelShader = nullptr;

Matrix gWorld;
Matrix gView;
Matrix gProj;

GfxParamsPerObj gGfxData;
ID3D11Buffer* gpGfxDataConstsBuffer;

Vector3 gCamPos = { 0, 0, -10 };
float gFOV = 0.25f*PI;

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

void UpdateConstsPerObj()
{
	gGfxData.wvp = gWorld * gView * gProj;
	WinUtil::Get().GetD3D().GetDeviceCtx().UpdateSubresource(gpGfxDataConstsBuffer, 0, nullptr, &gGfxData, 0, 0);
}

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

float GetRandom(float min, float max)
{
	float alpha = (float)rand() / RAND_MAX;
	alpha *= max - min;
	alpha += min;
	return alpha;
}


void InitGame()
{
	BuildGeometryBuffers();
	BuildFX();

	gWorld = Matrix::Identity;
	gView = Matrix::Identity;
	gProj = Matrix::Identity;

	srand(0);
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


void Update(float dTime)
{
	CreateProjectionMatrix(gProj, gFOV, WinUtil::Get().GetAspectRatio(), 1, 1000.f);
	CreateViewMatrix(gView, gCamPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
}

void Render(float dTime)
{
	MyD3D& d3d = WinUtil::Get().GetD3D();
	d3d.BeginRender(Colours::Blue);

	d3d.InitInputAssembler(gInputLayout, gBoxVB, sizeof(VertexPosColour), gBoxIB);
	d3d.GetDeviceCtx().VSSetShader(pVertexShader, nullptr, 0);
	d3d.GetDeviceCtx().PSSetShader(pPixelShader, nullptr, 0);

	Matrix trans = Matrix::CreateTranslation(0, 0, 0);
	gWorld = trans;
	UpdateConstsPerObj();
	d3d.GetDeviceCtx().VSSetConstantBuffers(0, 1, &gpGfxDataConstsBuffer);
	d3d.GetDeviceCtx().DrawIndexed(6, 0, 0);

	d3d.EndRender();
}

void OnResize(int screenWidth, int screenHeight, MyD3D& d3d)
{
	CreateProjectionMatrix(gProj, gFOV, WinUtil::Get().GetAspectRatio(), 1, 1000.f);
	d3d.OnResize_Default(screenWidth, screenHeight);
}


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
		}
	}

	//default message handling (resize window, full screen, etc)
	return WinUtil::Get().DefaultMssgHandler(hwnd, msg, wParam, lParam);
}

//main entry point for the game
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{

	int w(512), h(256);
	//int defaults[] = { 640,480, 800,600, 1024,768, 1280,1024 };
		//WinUtil::ChooseRes(w, h, defaults, 4);
	if (!WinUtil::Get().InitMainWindow(w, h, hInstance, "Fezzy", MainWndProc, true))
		assert(false);

	MyD3D d3d;
	if (!d3d.InitDirect3D(OnResize))
		assert(false);
	WinUtil::Get().SetD3D(d3d);
	d3d.GetCache().SetAssetPath("data/");

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

