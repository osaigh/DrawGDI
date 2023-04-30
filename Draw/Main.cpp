#include <windows.h>
#include <tchar.h>

POINT* apt;
LPCWSTR g_szClassName = L"myWindowClass";
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow) {
    WNDCLASSEX wndclass;
    HWND hwnd;
    MSG msg;

    //Register wind class
    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = g_szClassName;
    wndclass.hIconSm = NULL;

    if (!RegisterClassEx(&wndclass)) {
        MessageBox(NULL, TEXT("Window Registration Failed!"), g_szClassName,
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    //Draw window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        TEXT("DrawEditor"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    //Show window
    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    //message loop
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

void DrawPolyline(HDC hdc, POINT * apt, int cpt)
{
    Polyline(hdc, apt, cpt);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    static int cpt;
    HDC          hdc;
    int          cxClient, cyClient;
    PAINTSTRUCT  ps;
    HPEN           hpen;
    POINT startPoint;

    switch (msg)
    {
    case WM_SIZE:
        //Get the size of the client area
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);

        return 0;

    case WM_LBUTTONDOWN:
    {
        //if this is not the first WM_LBUTTONDOWN, we delete the previous memory allocated to the array
        if (cpt != 0) {
            delete[] apt;
        }
        cpt = 1;
        
        //store the location of the mouse. This will be the start if the new polyline
        apt = new POINT[1];
        apt[0].x = LOWORD(lParam);
        apt[0].y = HIWORD(lParam);
       
    }
    return 0;


    case WM_MOUSEMOVE:
        if (wParam & MK_LBUTTON)
        {
            //Get the device context for the window
            hdc = GetDC(hwnd);

            //If we have previously drawn on the window
            if (cpt > 1) {
                SelectObject(hdc, GetStockObject(WHITE_PEN));
                DrawPolyline(hdc, apt, cpt);
            }

            cpt += 1;
            const int iArrSize = cpt;
            POINT* pTemp = new POINT[iArrSize];

            //copy previous points into new POINT array
            int i = 0;
            for (i = 0; i < cpt - 1; i++) {
                pTemp[i].x = apt[i].x;
                pTemp[i].y = apt[i].y;
            }

            //Add the new point
            pTemp[cpt - 1].x = LOWORD(lParam);
            pTemp[cpt - 1].y = HIWORD(lParam);

            //free the memory from the previous POINT array
            delete[] apt;
            apt = pTemp;


            SelectObject(hdc, GetStockObject(BLACK_PEN));
            DrawPolyline(hdc, apt, cpt);
            ReleaseDC(hwnd, hdc);

        }
        return 0;

    case WM_PAINT:

        if (cpt > 1) {
            hdc = BeginPaint(hwnd, &ps);

            DrawPolyline(hdc, apt, cpt);

            EndPaint(hwnd, &ps);
        }

        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}