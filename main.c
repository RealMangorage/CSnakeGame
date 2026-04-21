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

HBRUSH background;
PAINTSTRUCT ps;

Size getOptimalSize(int maxWidth, int maxHeight, int tileSize)
{
    Size result;

    result.width  = (maxWidth  / tileSize) * tileSize;
    result.height = (maxHeight / tileSize) * tileSize;

    return result;
}

void updateCycle(HWND hwnd, UINT msg, UINT_PTR id, DWORD time) {
    Snake_Main_Update(hwnd);
    InvalidateRect(hwnd, NULL, TRUE);  // request redraw
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

    Snake_SetSize(size);
    Snake_SetDimensions(dimensions.width, dimensions.height);
    Snake_Init();

    HWND hwnd = CreateWindowEx(
            0,
            CLASS_NAME,
            L"Snake Game -> Score: 0",
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

    background = CreateSolidBrush(RGB(128, 128, 128));

    ShowWindow(hwnd, nCmdShow);
    SetTimer(hwnd, 1, 16, updateCycle); // 16ms per tick

    MSG msg = {};
    while (GetMessage(&msg, hwnd, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


void Main_Dispose() {
    Snake_Dispose();
    DeleteObject(background);
    DeleteObject(&ps);
}

void handleKeyEvent(WPARAM wParam, int keyDown) {
    switch (wParam)
    {
        case VK_UP:
            Snake_SetDirection(UP);
            break;

        case VK_DOWN:
            Snake_SetDirection(DOWN);
            break;

        case VK_LEFT:
            Snake_SetDirection(LEFT);
            break;

        case VK_RIGHT:
            Snake_SetDirection(RIGHT);
            break;

        case VK_ESCAPE:
            Main_Dispose();
            exit(0);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
            KillTimer(hwnd, 1);        // FIX: stop timer
            Main_Dispose();
            PostQuitMessage(0);
            return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);       // FIX: proper shutdown path
            return 0;

        case WM_SETCURSOR:
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return TRUE;

        case WM_KEYDOWN:
            handleKeyEvent(wParam, 1);
            return 0;

        case WM_ERASEBKGND:
            return 1; // FIX: prevents flicker

        case WM_PAINT:
        {
            HDC hdc = BeginPaint(hwnd, &ps);

            FillRect(hdc, &ps.rcPaint, background);

            Snake_Render(hdc);

            EndPaint(hwnd, &ps);
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
