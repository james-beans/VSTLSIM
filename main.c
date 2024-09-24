#include <windows.h>
#include <stdbool.h>
#include <time.h>

#define RED_LIGHT 0
#define YELLOW_LIGHT 1
#define GREEN_LIGHT 2

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void DrawTrafficLight(HDC hdc, int currentLight);
void ChangeLight(HWND hwnd);

int currentLight = RED_LIGHT;
bool running = true;
clock_t lastChangeTime;
double delayTime = 1.0;  // Default delay time in seconds

void UpdateDelayTime() {
    if (currentLight == GREEN_LIGHT) {
        delayTime = 5.25;
    } else {
        delayTime = 1.0;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[]  = "TrafficLightSimulator";

    WNDCLASS wc = { 0 }; // Properly initialize with 0

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        "Traffic Light Simulator",      // Window text
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,  // Window style (non-resizable)

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 200, 400,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    lastChangeTime = clock();

    // Run the message loop.
    MSG msg = { 0 };
    while (running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        clock_t currentTime = clock();
        double elapsedTime = (double)(currentTime - lastChangeTime) / CLOCKS_PER_SEC;
        if (elapsedTime >= delayTime) {
            ChangeLight(hwnd);
            lastChangeTime = clock();
        }

        Sleep(100);  // Sleep for a short time to prevent high CPU usage
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            running = false;
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.
            DrawTrafficLight(hdc, currentLight);

            EndPaint(hwnd, &ps);
        }
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void DrawTrafficLight(HDC hdc, int currentLight) {
    RECT rect;
    GetClientRect(GetForegroundWindow(), &rect);

    int lightDiameter = (rect.bottom - rect.top) / 4;
    int lightSpacing = lightDiameter / 4;

    int centerX = (rect.right - rect.left) / 2;
    int centerY = lightDiameter / 2;

    // Draw the red light
    HBRUSH hBrush = (currentLight == RED_LIGHT) ? CreateSolidBrush(RGB(255, 0, 0)) : CreateSolidBrush(RGB(100, 0, 0));
    SelectObject(hdc, hBrush);
    Ellipse(hdc, centerX - lightDiameter / 2, centerY - lightDiameter / 2, centerX + lightDiameter / 2, centerY + lightDiameter / 2);
    DeleteObject(hBrush);

    // Draw the yellow light
    centerY += lightDiameter + lightSpacing;
    hBrush = (currentLight == YELLOW_LIGHT) ? CreateSolidBrush(RGB(255, 255, 0)) : CreateSolidBrush(RGB(100, 100, 0));
    SelectObject(hdc, hBrush);
    Ellipse(hdc, centerX - lightDiameter / 2, centerY - lightDiameter / 2, centerX + lightDiameter / 2, centerY + lightDiameter / 2);
    DeleteObject(hBrush);

    // Draw the green light
    centerY += lightDiameter + lightSpacing;
    hBrush = (currentLight == GREEN_LIGHT) ? CreateSolidBrush(RGB(0, 255, 0)) : CreateSolidBrush(RGB(0, 100, 0));
    SelectObject(hdc, hBrush);
    Ellipse(hdc, centerX - lightDiameter / 2, centerY - lightDiameter / 2, centerX + lightDiameter / 2, centerY + lightDiameter / 2);
    DeleteObject(hBrush);
}

void ChangeLight(HWND hwnd) {
    currentLight = (currentLight + 1) % 3;
    UpdateDelayTime();
    InvalidateRect(hwnd, NULL, TRUE);
}
