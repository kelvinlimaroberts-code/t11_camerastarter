#ifndef D3DH
#define D3DH

#include <d3d11.h>
#include "SimpleMath.h"
#include "TexCache.h"

/*
* wrap common D3D behaviour behind a simpler interface
*/
class MyD3D
{
public:

	/*
	main start up function
	pOnResize - function pointer to something we can call when we detect
				the screen has changed resolution
	return - true if there were no problems
	*/
	bool InitDirect3D(void(*pOnResize)(int, int, MyD3D&));
	//default minimum behaviour when ALT+ENTER or drag or resize
	//parameters are new width and height of window
	void OnResize_Default(int clientWidth, int clientHeight);
	//main shutdown function, don't forget to call it
	//extraReporting gives a bit more information about any objects
	//that we haven't released properly
	void ReleaseD3D(bool extraReporting = true);
	//is the screen/window square or letterbox or?
	float GetAspectRatio();
	
	/*
	* wipe the back buffer prior to a new render cycle starting
	* colour - colour to wip the buffer
	*/
	void BeginRender(const DirectX::SimpleMath::Vector4& colour);
	//tell the gpu to swap front and back buffers when ready
	void EndRender();

	/*
	* Accessors
	*/
	ID3D11Device& GetDevice() { 
		assert(mpd3dDevice);
		return *mpd3dDevice;
	}
	ID3D11DeviceContext& GetDeviceCtx() {
		assert(mpd3dImmediateContext);
		return *mpd3dImmediateContext;
	}
	bool GetDeviceReady() const {
		return mpd3dDevice!=nullptr;
	}
	TexCache& GetCache() { return mTexCache; }
	ID3D11SamplerState& GetWrapSampler() {
		assert(mpWrapSampler);
		return *mpWrapSampler;
	}

	//see mpOnResize - just checks it's not null before calling it
	void OnResize(int sw, int sh, MyD3D& d3d) {
		assert(mpOnResize);
		mpOnResize(sw, sh, d3d);
	}

	/*
	* Prepare the input assembler on the graphics pipeline for new data
	* pInputLayout - vertex description D3D object
	* pVBuffer - vertex buffer
	* szVertex - size of a vertex
	* pIBuffer - index buffer
	* topology - what do these buffers refer to? Lines, points, triangle lists?
	*/
	void InitInputAssembler(ID3D11InputLayout* pInputLayout, ID3D11Buffer* pVBuffer, UINT szVertex, ID3D11Buffer* pIBuffer, 
								D3D_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


private:
	//library of unique textures, only load one of each once, never duplicate
	TexCache mTexCache;
	//what type of gpu have we got - hopefully a hardware one
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_UNKNOWN;
	//texture multisampling quality level supported
	UINT m4xMsaaQuality=0;
	//depth buffer for sorting pixels by distance from camera
	ID3D11Texture2D* mpDepthStencilBuffer=nullptr;
	bool mEnable4xMsaa = true;
	//running in a window?
	bool mWindowed = false;
	//main handle used to create resources and access D3D
	ID3D11Device* mpd3dDevice = nullptr;
	//a handle off the device we can use to give rendering commands
	ID3D11DeviceContext* mpd3dImmediateContext = nullptr;
	//a number of surfaces we can render onto and then display
	IDXGISwapChain* mpSwapChain = nullptr;
	//when we render things, where do the go?
	ID3D11RenderTargetView* mpRenderTargetView = nullptr;
	//when rendering we can test the depth of pixels, usually so we
	//avoid rendering anything that is behind something else
	ID3D11DepthStencilView* mpDepthStencilView = nullptr;
	//position, height, width, min+max depth of the view we are rendering
	D3D11_VIEWPORT mScreenViewport;
	//a function to call when we ALT+ENTER or drag the window
	//two parameters are width/height of the new window and this
	void(*mpOnResize)(int, int, MyD3D&) = nullptr;
	//D3D object to configure wrapping when sampling a texture
	ID3D11SamplerState* mpWrapSampler = nullptr;

	//heavy lifting to start D3D11
	void CreateD3D(D3D_FEATURE_LEVEL desiredFeatureLevel = D3D_FEATURE_LEVEL_11_0);
	//buffers in the swap chain must match the screen resolution
	void ResizeSwapChain(int screenWidth, int screenHeight);
	//the kind of depth stencil we want
	void CreateDepthStencilDescription(D3D11_TEXTURE2D_DESC& dsd, int screenWidth, int screenHeight, bool msaa4X, int maxQuality);
	//create all render buffers
	void CreateDepthStencilBufferAndView(D3D11_TEXTURE2D_DESC& dsd);
	// Bind the render target view and depth/stencil view to the pipeline.
	void BindRenderTargetViewAndDepthStencilView();
	//viewport dimensions need updating when the window changes size
	void SetViewportDimensions(int screenWidth, int screenHeight);
	//can this gpu support 4x sampling
	void CheckMultiSamplingSupport(UINT& quality4xMsaa);
	//specify the type of swapchain needed
	void CreateSwapChainDescription(DXGI_SWAP_CHAIN_DESC& sd, HWND hMainWnd, bool windowed, int screenWidth, int screenHeight);
	//once you have a description you can create the swapchain needed
	void CreateSwapChain(DXGI_SWAP_CHAIN_DESC& sd);
	//make a D3D texture wrapping sampler object
	void CreateWrapSampler(ID3D11SamplerState* &pSampler);

};



#endif
