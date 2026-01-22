#include "DeviceResources.h"
#include <stdexcept>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using namespace DX;
using Microsoft::WRL::ComPtr;

DeviceResources::DeviceResources() {}

void DeviceResources::SetWindow(HWND window, int width, int height)
{
    m_window = window;
    m_width = width;
    m_height = height;
}

void DeviceResources::CreateDeviceResources()
{
    UINT creationFlags = 0;
#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    D3D_FEATURE_LEVEL featureLevel;

    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        1,
        D3D11_SDK_VERSION,
        m_d3dDevice.GetAddressOf(),
        &featureLevel,
        m_d3dContext.GetAddressOf()
    );

    if (FAILED(hr))
        throw std::runtime_error("D3D11CreateDevice failed");
}

void DeviceResources::CreateWindowSizeDependentResources()
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = m_width;
    sd.BufferDesc.Height = m_height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_window;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;

    ComPtr<IDXGIDevice> dxgiDevice;
    m_d3dDevice.As(&dxgiDevice);

    ComPtr<IDXGIAdapter> adapter;
    dxgiDevice->GetAdapter(&adapter);

    ComPtr<IDXGIFactory> factory;
    adapter->GetParent(__uuidof(IDXGIFactory), &factory);

    HRESULT hr = factory->CreateSwapChain(
        m_d3dDevice.Get(),
        &sd,
        m_swapChain.GetAddressOf()
    );
    if (FAILED(hr)) throw std::runtime_error("CreateSwapChain failed");

    ComPtr<ID3D11Texture2D> backBuffer;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
    m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = m_width;
    depthDesc.Height = m_height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ComPtr<ID3D11Texture2D> depthBuffer;
    m_d3dDevice->CreateTexture2D(&depthDesc, nullptr, depthBuffer.GetAddressOf());
    m_d3dDevice->CreateDepthStencilView(depthBuffer.Get(), nullptr, m_depthStencilView.GetAddressOf());

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    D3D11_VIEWPORT vp = {};
    vp.Width = float(m_width);
    vp.Height = float(m_height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_d3dContext->RSSetViewports(1, &vp);
}

void DeviceResources::Clear(bool failed)
{
    float color[4];

    if (failed)
    {
        color[0] = 1.0f; // Ô
        color[1] = 0.0f;
        color[2] = 0.0f;
        color[3] = 1.0f;
    }
    else
    {
        color[0] = 0.1f;
        color[1] = 0.1f;
        color[2] = 0.3f;
        color[3] = 1.0f;
    }

    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), color);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DeviceResources::Present()
{
    m_swapChain->Present(1, 0);
}
