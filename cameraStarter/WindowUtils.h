#ifndef WINDOWSUTILS
#define WINDOWSUTILS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <sstream>
#include <assert.h>

class MyD3D;

/*
* Helper object that takes care of the operating system and setting up a window
* Only ever want one such object
*/
class WinUtil
{
public:
	//get rid of these
	WinUtil(WinUtil const&) = delete;
	void operator=(WinUtil const&) = delete;
	//singleton getter
	static WinUtil& Get()
	{ 
		static WinUtil instance; 
		return instance;
	}
	/*
	* the operating system will send us messages and we must respond to some at least
	* hwnd - IN handle to the window the operating system thinks it is talking to
	* msg - an event ID so we know what kind of message is incoming
	* wParam/lParam - extra data the O.S. may be using to hold more information
	* returns - a return value that is message dependant 
	*/
	static LRESULT DefaultMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return WinUtil::Get().RealDefaultMssgHandler(hwnd, msg, wParam, lParam);
	}
	/*
	* open a window
	* width/height - IN desired resolution of the window
	* hInstance - IN a handle to the program so the O.S. can attach the window to the exe
	  appName - INT name to show on the window
	  mssgHandler - IN function pointer to the O.S. message handler
	  centred - IN middle of the monitor or not?
	*/
	bool InitMainWindow(int width, int height, HINSTANCE hInstance, const std::string& appName, WNDPROC mssgHandler, bool centred=true);
	//this object needs to be able to pass messages to D3D if the resolution changes
	void SetD3D(MyD3D& d3d) {
		assert(mpMyD3D == nullptr);
		mpMyD3D = &d3d;
	}
	MyD3D& GetD3D() {
		assert(mpMyD3D);
		return *mpMyD3D;
	}
	/*wrap your game in one function call if using basic functions
	* Easy, but awkward to have free functions for update/render
	  or even rework the Run function using std::function and lamdas to wrap the member function with a this pointer - advanced
	* pUpdate - IN pointer to your own update function, gives elapsed time as a parameter
	* pRender - IN same but for rendering
	*/
	int Run(void(*pUpdate)(float), void(*pRender)(float));
	
	/*if you are using class member functions then try this instead
	* BeginLoop - use this in a while loop
	* canUpdateRender - OUT if this ends up true then the game has lost focus so you might want to stop updating
	* returns - true if it's OK to keep looping
	*/
	bool BeginLoop(bool& canUpdateRender);
	/*
	* EndLoop - use after BeginLoop when the update is complete
	* didUpdateRender - IN if we did run and update/render then let this function know
	* returns - how long one update/render took
	*/
	float EndLoop(bool didUpdateRender);


	//getters
	//*******
	//handle to our exe
	HINSTANCE GetAppInst();
	//handle to our window
	HWND GetMainWnd();
	//width and height of window
	void GetClientExtents(int& width, int& height) const;
	int GetClientWidth() const {
		return mWinData.clientWidth;
	}
	int GetClientHeight() const {
		return mWinData.clientHeight;
	}
	//what is the aspect ratio?
	float GetAspectRatio();
	/*
	* if you are trying to only use certain resolutions, use this to pick the highest that still fits 
	* w - OUT width
	* h - OUT height
	* defaults - IN an array of w/h pairs
	* numPairs - IN how many in the array
	*/
	void ChooseRes(int& w, int& h, int defaults[], int numPairs);

private:
	struct WinData
	{
		//all windows apps have these handles
		HINSTANCE hAppInst = 0;			//exe handle
		HWND      hMainWnd = 0;			//window handle
		bool      appPaused = false;	//lost focus so pause please
		bool      minimized = false;	//minimised the window
		bool      maximized = false;	//maximised the window
		bool      resizing = false;		//dragged the window edges to change size
		std::string mainWndCaption;		//title
		int clientWidth;				//resolution
		int clientHeight;				//resolution
	};
	WinData mWinData;	//data describing what's going on
	MyD3D *mpMyD3D;		//handle to the local d3d object
	
	//signleton so constructor is private
	WinUtil() 
		:mpMyD3D(nullptr) 
	{}

	/*
	watch for basic O.S. events and update the mWinData items and monitor the resolution
	to keep the D3D buffers in sync (same size)
	*/
	LRESULT RealDefaultMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

#endif
