#include <tchar.h>
#include <iostream>

#include <fcntl.h>
#include <io.h>
#include "Renderer.cpp"

#define PIXELSIZE 3;

enum saveMode {MODE_NONE, MODE_PREVIEW, MODE_NORMAL};

saveMode mode = MODE_NONE;
static int width;
static int height; 
static int texWidth;
static int texHeight; 
static double oldX = 0;
static double oldY = 0;
static HWND windowHandle;
Renderer renderer;



// Forward declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool handleMouseMovement(int x, int y);
void* generateNoise();
void handleCmdLine(LPSTR lpCmdLine);
void showConsole();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//showConsole();

	char *cmd = lpCmdLine;

	if (*cmd=='\"') cmd++;
	if (*cmd=='/'||*cmd=='-') cmd++; 
	char command = cmd[0];
	
	cmd++;
	if(*cmd==':' || *cmd==' ')
		cmd++;
	int handle = atoi(cmd);
		
	//std::cout << " -> " << command << " and " << handle << std:: endl<< std:: endl; 
	
	if (command=='c'||command=='C') 
	{
		LPCSTR title = "Keine Einstellungen";
		LPCSTR message = "Dieser Bildschirmschoner hat keine Einstellungsmöglichkeiten";
		MessageBox(NULL,message,title,MB_OK);
		return 0;
	}
	else if (command=='s'||command=='S'||command==0)
	{
		mode = MODE_NORMAL;
		width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

		Window w = Window(_T("test"), hInstance, WndProc, nCmdShow, width, height, NULL, false);
		windowHandle = w.getHwnd();
		ShowCursor(false);
	}
	else if (command=='p'||command=='P')
	{
		mode = MODE_PREVIEW;

		HWND parent = (HWND)handle;
		RECT wndSize;
		GetWindowRect(parent, &wndSize);
		width = wndSize.right-wndSize.left;
		height = wndSize.bottom-wndSize.top;

		Window w = Window(_T("test"), hInstance, WndProc, nCmdShow, width, height, parent, true);
		windowHandle = w.getHwnd();
		
		
		//windowHandle=CreateWindowEx(0,"prev","SaverPreview",WS_CHILD|WS_VISIBLE,0,0,width,height,(HWND)handle,NULL,hInstance,NULL);
		

		//windowHandle = (HWND)handle;
		//RECT wndSize;
		//GetWindowRect(windowHandle, &wndSize);
		//width = wndSize.right-wndSize.left;
		//height = wndSize.bottom-wndSize.top;


	}

	if(mode==MODE_NORMAL||mode==MODE_PREVIEW)
	{
		texWidth = width / PIXELSIZE;
		texHeight = height / PIXELSIZE; 

		renderer = Renderer(windowHandle, width, height);	
		renderer.InitGL();
	}
	
	// Main message loop:
    MSG msg;
    while( 1 )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
			//std::cout << msg.message << std::endl;
			if( msg.message == WM_QUIT || msg.message == WM_CLOSE)
            {
                return 1;
            }
            
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {	
			if(mode==MODE_NORMAL||mode==MODE_PREVIEW)
			{
				void* noise = generateNoise();
				renderer.draw(noise, texWidth, texHeight);
				delete noise;
			}
			Sleep(10);
        }
    }
	
	if(mode==MODE_NORMAL||mode==MODE_PREVIEW)
	{
		renderer.CloseGL();
	}
    
	return msg.wParam;
}


void* generateNoise()
{
	float *arr = new float[texWidth*texHeight];
	
	//float mid = 0;

	for(int i = 0; i < texWidth*texHeight; i++)
	{
		int random = rand() % 255;
		arr[i] = random/255.0f; // arr[i+1] = arr[i+2] = random/255.0f;
		//mid += random/255.0f;
	}

	//arr[texWidth*texHeight-1] = 255;
	//std::cout << mid/(texWidth*texHeight) << std::endl;
		
	return arr;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//std::cout << message << std::endl;
	switch (message)
	{
		case WM_CREATE:
			break;

		case WM_PAINT:
			{
				//HDC hScreenDC = CreateDC("DISPLAY", NULL, NULL, NULL);     
				//BitBlt(hdc2, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);

				break;
			}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_MOUSEWHEEL:
		case WM_KEYDOWN:
			if(mode==MODE_NORMAL)
				PostQuitMessage(0);
			break;
		case WM_DESTROY:
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		
		case WM_MOUSEMOVE:
			if(!handleMouseMovement(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam))&&mode==MODE_NORMAL)
			{
				PostQuitMessage(0);
				break;
			}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool handleMouseMovement(int x, int y)
{
	if(oldX == 0 && oldY == 0)
	{
		oldX = x;
		oldY = y;
	}
	
	double distance = sqrt((oldX - x)*(oldX - x) + (oldY - y)*(oldY - y));
	if(distance > 10.0)
	{
		return 0;
	}
	else
		return 1;
}

void showConsole()
{
	AllocConsole();

    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;
}