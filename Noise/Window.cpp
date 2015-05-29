#include <windows.h>
#include <Windowsx.h>
#include <math.h>
class Window {
private :

    HWND hwnd;

    int width;
    int height;
    HINSTANCE hInst;
    TCHAR *Class;

public:

    Window::Window(TCHAR *name, HINSTANCE hInstance, WNDPROC wndProc, int nCmdShow, int width, int height, HWND parent,
                   bool preview) {

        this->width = width;
        this->height = height;

        Class = name;

        WNDCLASS wc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hInstance = hInstance;
        wc.lpfnWndProc = wndProc;
        wc.lpszClassName = Class;
        wc.lpszMenuName = 0;
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

        // Register that class with the Windows O/S..
        RegisterClass(&wc);

        hInst = hInstance;

        if (preview) {
            hwnd = CreateWindowEx(
                    0,
                    Class,
                    "SaverPreview",
                    WS_CHILD | WS_VISIBLE,
                    0,
                    0,
                    width,
                    height,
                    parent,
                    NULL,
                    hInstance,
                    NULL);
        }
        else {
            hwnd = CreateWindow(
                    Class,
                    "NoiseSaver",
                    WS_OVERLAPPED | WS_POPUP,
                    GetSystemMetrics(SM_XVIRTUALSCREEN),
                    GetSystemMetrics(SM_YVIRTUALSCREEN),
                    width, height,
                    parent,
                    NULL,
                    hInstance,
                    NULL
            );
        }

        ShowWindow(hwnd, nCmdShow);

    }

    Window::~Window(void) {

    }

    HWND Window::getHwnd() {
        return hwnd;
    }

    int Window::getWidth() {
        return width;
    }

    int Window::getHeight() {
        return height;
    }
};