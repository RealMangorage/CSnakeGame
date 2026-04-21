#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include "snake.h"

typedef struct {
    int width;
    int height;
} Size;

Size getOptimalSize(int maxWidth, int maxHeight, int tileSize)
{
    Size result;

    result.width  = (maxWidth  / tileSize) * tileSize;
    result.height = (maxHeight / tileSize) * tileSize;

    return result;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
//    AllocConsole();
//    freopen("CONOUT$", "w", stdout);

    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Snake Game";

    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    int size = 20;
    Size dimensions = getOptimalSize(800, 600, size);

    setSize(size);
    setDimensions(dimensions.width, dimensions.height);
    resetSnake();

    HWND hwnd = CreateWindowEx(
            0,
            CLASS_NAME,
            L"Snake Game",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
            CW_USEDEFAULT, CW_USEDEFAULT,
            dimensions.width, dimensions.height,   // <-- width, height
            NULL,
            NULL,
            hInstance,
            NULL
    );


    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    SetTimer(hwnd, 1, 1/20, NULL); // 10ms per tick

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_CLOSE:
        {
            exit(0);
        }
        case WM_SETCURSOR:
        {
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return TRUE;
        }
        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_UP:
                    setDirection(UP);
                    break;

                case VK_DOWN:
                    setDirection(DOWN);
                    break;

                case VK_LEFT:
                    setDirection(LEFT);
                    break;

                case VK_RIGHT:
                    setDirection(RIGHT);
                    break;
                case VK_F7:
                    grow();
                    break;

                case VK_ESCAPE:
                    exit(0);
            }
            return 0;
        }
        case WM_KEYUP: {
            return 0;
        }
        case WM_TIMER:
        {
            update();
            InvalidateRect(hwnd, NULL, TRUE);  // request redraw
            return 0;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
            render(hdc);

            EndPaint(hwnd, &ps);
        }
        return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}