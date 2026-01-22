#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>
namespace DX
{
    class DeviceResources
    {
    public:
        DeviceResources();

        void SetWindow(HWND window, int width, int height);
        void CreateDeviceResources();
        void CreateWindowSizeDependentResources();

        void Clear(bool failed = false);
        void Present();

        ID3D11Device* GetD3DDevice() const { return m_d3dDevice.Get(); }
        ID3D11DeviceContext* GetD3DDeviceContext() const { return m_d3dContext.Get(); }

    private:
        HWND m_window = nullptr;
        int m_width = 800;
        int m_height = 600;

        Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext;
        Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    };
}
