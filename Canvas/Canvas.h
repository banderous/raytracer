#pragma once

#include "Windows.h"
#include "ddraw.h"
#include <string>

namespace CanvasContainer
{
	using namespace std;

	class Canvas
	{
		public:
			~Canvas();
			Canvas(string title, unsigned int resX, unsigned int resY, bool fullScreen);

			// Render functions
			void StartRender();
			void FinishAndDisplayRender();
			void SetPixel(int x, int y, int colour);
			int GetPixel(int x, int y);
			void Setup();
			bool WriteBitmap(string filepath);

		private:
			char** MakeRGBFromScreen(DDSURFACEDESC2* desc);
			int SetupDirectDraw();
			void SetupWindow();

			HANDLE msgPumpThreadHandle;

			HWND hWnd;
			int resX, resY;
			bool fullScreen;
			string title;

			// Direct draw stuff
			LPDIRECTDRAW7 dDrawObject; 
			LPDIRECTDRAWSURFACE7 primarySurface; 
			LPDIRECTDRAWSURFACE7 backSurface; 
			LPDIRECTDRAWCLIPPER wndClipper;
			DDSURFACEDESC2 desc;
	};
}