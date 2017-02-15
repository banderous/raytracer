#include "Canvas.h"

namespace CanvasContainer
{
	bool signal = false;

	/*
		Pump messages for the specified window
	*/
	static void MessagePump(Canvas* ignore)
	{
		ignore->Setup();
		signal = true;

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Canvas::~Canvas()
	{
		this->backSurface->Release();
		this->primarySurface->Release();
		this->dDrawObject->Release();
		this->wndClipper->Release();
		DestroyWindow(hWnd);
	}

	Canvas::Canvas(string title, unsigned int resX, unsigned int resY, bool fullScreen)
	{
		this->resX = resX;
		this->resY = resY;
		this->title = title;
		this->fullScreen = fullScreen;
		

		this->msgPumpThreadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MessagePump, this, 0, 0); //start the thread to listen for incoming connections
		while(!signal)
		{
			Sleep(100);
		}
	}

	LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	void Canvas::Setup()
	{
		if (fullScreen)
		{
			throw -1;
		}
		else
		{
			this->SetupWindow();
			this->SetupDirectDraw();
		}
	}

	void Canvas::SetupWindow()
	{

		WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, 
						GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
						title.c_str(), NULL };

		ATOM a = RegisterClassEx( &wc );

		hWnd = CreateWindow(title.c_str(), title.c_str(), 
						WS_OVERLAPPEDWINDOW, 0, 0, this->resX, this->resY,
							GetDesktopWindow(), NULL, wc.hInstance, NULL );	
		ShowWindow( hWnd, SW_SHOWDEFAULT );
	}

	int Canvas::SetupDirectDraw()
	{
		HRESULT dDrawResult;
   		dDrawResult = DirectDrawCreateEx(NULL,(VOID **)&dDrawObject, IID_IDirectDraw7, NULL);
		if( dDrawResult != DD_OK )  
			throw -1; 

		dDrawResult = dDrawObject->SetCooperativeLevel( hWnd, DDSCL_NORMAL);
		
		if( dDrawResult != DD_OK )
			throw -1;

		DDSURFACEDESC2 ddsd;
		memset( &ddsd, 0, sizeof(ddsd) );
		ddsd.dwSize = sizeof( ddsd );

		ddsd.dwFlags = DDSD_CAPS ;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		ddsd.dwBackBufferCount = 1;

		dDrawResult = dDrawObject->CreateSurface( &ddsd, &primarySurface, NULL );
		if( dDrawResult != DD_OK )
		{
			dDrawObject->Release();
			throw -1;
		}


		dDrawResult = dDrawObject->CreateClipper( 0, &wndClipper, NULL );
		if( dDrawResult != DD_OK )
		{
			primarySurface->Release();
			dDrawObject->Release();
			throw -1;
		}
          
		// setting it to our hwnd gives the clipper the coordinates from our window
		dDrawResult = wndClipper->SetHWnd( 0, hWnd );
		if( dDrawResult != DD_OK )
		{
			wndClipper-> Release();
			primarySurface->Release();
			dDrawObject->Release();
			throw -1;
		}

		dDrawResult = primarySurface->SetClipper(wndClipper);
		if( dDrawResult != DD_OK )
		{
			wndClipper-> Release();
			primarySurface->Release();
			dDrawObject->Release();
			throw -1;
		}

		memset( &ddsd, 0, sizeof(ddsd) );
		ddsd.dwSize = sizeof( ddsd );
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;	//keep her in system memory, faster to lock
		ddsd.dwWidth = resX;
		ddsd.dwHeight = resY;

		// create the backbuffer separately
		dDrawResult = dDrawObject->CreateSurface( &ddsd, &backSurface, NULL );
		if( dDrawResult != DD_OK )
		{
			wndClipper-> Release();
			primarySurface->Release();
			dDrawObject->Release();
			throw -1;
		}

		return 0;
	}

	void Canvas::StartRender()
	{
		DDBLTFX fx;
		fx.dwSize = sizeof(fx);
		fx.dwFillColor = 0;

		backSurface->Blt(NULL, NULL, NULL, DDBLT_ASYNC | DDBLT_COLORFILL, &fx); 

		memset(&desc,0,sizeof(desc));
		desc.dwSize = sizeof(desc);

		if (!SUCCEEDED(backSurface->Lock(NULL,&desc,DDLOCK_WAIT,NULL)))
		{
			throw -1;
		}
	}

	void Canvas::FinishAndDisplayRender()
	{
		backSurface->Unlock(NULL);	

		RECT rcRectSrc;
		RECT rcRectDest;
		POINT p;
		p.x = 0; p.y = 0;
		ClientToScreen(hWnd, &p);
		GetClientRect(hWnd, &rcRectDest);
		OffsetRect(&rcRectDest, p.x, p.y);
		SetRect(&rcRectSrc, 0, 0, this->resX, this->resY);
		HRESULT result = primarySurface->Blt( &rcRectDest, backSurface, 0, DDBLT_DONOTWAIT, NULL);
	}

	void Canvas::SetPixel(int x, int y, int colour)
	{
		// Contiguous array of pixels. Add on 
		((LPVOID*)desc.lpSurface)[y * resX + x] = (LPVOID) colour;
	}

	int Canvas::GetPixel(int x, int y)
	{
		return (int) ((LPVOID*)desc.lpSurface)[y * resX + x];
	}

	/*
		Write the current display surface out to a bitmap
	*/
	bool Canvas::WriteBitmap(std::string filepath)
	{
		char** image = this->MakeRGBFromScreen(&this->desc);
		BITMAPFILEHEADER header;
		BITMAPINFO info;	

		ZeroMemory(&header, sizeof(BITMAPFILEHEADER));
		ZeroMemory(&info, sizeof(BITMAPINFO));

		header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO);
		header.bfSize = header.bfOffBits + resX * resY * 3;
		header.bfType = 19778;

		info.bmiHeader.biHeight = resY;
		info.bmiHeader.biWidth = resX;
		info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		info.bmiHeader.biPlanes = 1;
		info.bmiHeader.biBitCount = 24;
		info.bmiHeader.biCompression = BI_RGB;
		info.bmiHeader.biSizeImage = 0;
		info.bmiHeader.biClrImportant = 0;

		FILE* filepointer = NULL;
		fopen_s(&filepointer, filepath.c_str(), "wb");

		int first = sizeof(BITMAPFILEHEADER);
		int second = sizeof(BITMAPINFO);
		fwrite(&header, sizeof(BITMAPFILEHEADER), 1, filepointer);
		fwrite(&info, sizeof(BITMAPINFO), 1, filepointer);

		//fseek(filepointer, header.bfOffBits, SEEK_SET);

		char** inverter = new char*[info.bmiHeader.biHeight];
		for(int x = 0; x < info.bmiHeader.biHeight; x++)
			inverter[x] = new char[info.bmiHeader.biWidth*3];

		for(int y = 0; y < info.bmiHeader.biHeight; y++)
		{
			for(int x = 0; x < info.bmiHeader.biWidth * 3; x+=3)
			{
				inverter[y][x] = image[y][x];
				inverter[y][x+1] = image[y][x+1];
				inverter[y][x+2] = image[y][x+2];
			}
		}

		int count = 0;
		for(int y = info.bmiHeader.biHeight - 1; y >= 0 ; y--)
		{
			for(int x = 0; x < info.bmiHeader.biWidth * 3; x+=3)
			{
				BYTE red = inverter[y][x];
				BYTE green = inverter[y][x+1];
				BYTE blue = inverter[y][x+2];
				
				fwrite(&blue, 1, 1, filepointer);
				fwrite(&green, 1, 1, filepointer);
				fwrite(&red, 1, 1, filepointer);
				++count;
			}
		}
		fflush(filepointer);
		fclose(filepointer);
		
		for(int x = 0; x < info.bmiHeader.biHeight; x++)
			delete inverter[x];
		delete inverter;

		return true;
	}

	char** Canvas::MakeRGBFromScreen(DDSURFACEDESC2* desc)
	{
		char** newImage = new char*[desc->dwHeight];

		for(unsigned int x = 0; x < desc->dwHeight; x++)
			newImage[x] = new char[desc->dwWidth * 3];
		
		LPVOID *pixels = (LPVOID *)desc->lpSurface;
		int pitch = desc->lPitch;
		for(unsigned int y = 0; y < desc->dwHeight; y++)
		{
			int newX = 0;
			for(unsigned int x = 0; x < desc->dwWidth; x++)
			{
				int colour = GetPixel(x, y);
				BYTE red = (colour & 0xff0000) >> 16;
				BYTE green = (colour & 0xff00) >> 8;
				BYTE blue = colour & 0xff;

				newImage[y][newX] = red;
				newImage[y][newX+1] = green;
				newImage[y][newX+2] = blue;
				newX += 3;
			}
		}
		return newImage;
	}
}