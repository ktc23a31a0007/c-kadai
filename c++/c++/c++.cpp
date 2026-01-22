#include "framework.h"
#include "c++.h"

#include <windowsx.h>   // ← ★これを追加★

#include "DeviceResources.h"
#include "Game.h"


using namespace DX;

std::shared_ptr<DeviceResources> g_deviceResources;
std::unique_ptr<Game> g_game;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
    WNDCLASS wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"IrritationBar";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        wc.lpszClassName, L"Irritation Bar",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 800,
        nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hwnd, nCmdShow);

    g_deviceResources = std::make_shared<DeviceResources>();
    g_deviceResources->SetWindow(hwnd, 1280, 800);
    g_deviceResources->CreateDeviceResources();
    g_deviceResources->CreateWindowSizeDependentResources();

    g_game = std::make_unique<Game>(g_deviceResources);
    g_game->Initialize(hwnd, 1280, 800);

    MSG msg{};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            g_game->Update();
            g_deviceResources->Clear();
            g_game->Render();
            g_deviceResources->Present();
        }
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (g_game)
    {
        switch (message)
        {
        case WM_LBUTTONDOWN:
            SetCapture(hWnd);
            g_game->OnMouseDown(
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam),
                true
            );
            return 0;

        case WM_LBUTTONUP:
            ReleaseCapture();
            g_game->OnMouseUp(true);
            return 0;

        case WM_RBUTTONDOWN:
            SetCapture(hWnd);
            g_game->OnMouseDown(
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam),
                false
            );
            return 0;

        case WM_RBUTTONUP:
            ReleaseCapture();
            g_game->OnMouseUp(false);
            return 0;

        case WM_MOUSEMOVE:
            g_game->OnMouseMove(
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            );
            return 0;
        }
    }

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

