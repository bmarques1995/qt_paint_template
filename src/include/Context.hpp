#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <d3d11_4.h>
#include <wrl.h>
#include <cstdint>
#include <thread>
#include <mutex>

using Microsoft::WRL::ComPtr;

class Context final
{
public:
    Context(HWND windowHandle, uint32_t width, uint32_t height);
    virtual ~Context();

    void ClearTarget();
    void SetClearColor(float r, float g, float b, float a);
    void ReceiveCommands();
    void DispatchCommands();
    void Draw(uint32_t elements);
    void NewFrame();
    void EndFrame();
    void Present();

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetDeviceContext() const;

    void OnResize(uint32_t width, uint32_t height);

    // virtual std::any GetImGUIData() override;
private:
    void CreateDevice();
    void CreateSwapChain(HWND windowHandle);
    void CreateFactoryAndAdapter();
    void CreateRenderTarget();
    void CreateViewport(uint32_t width, uint32_t height);

    float m_ClearColor[4];

    bool m_Modifying;

    HWND m_WindowHandle;

    ComPtr<ID3D11Device> m_Device;
    ComPtr<ID3D11DeviceContext> m_DeviceContext;
    ComPtr<IDXGISwapChain> m_SwapChain;
    ComPtr<IDXGIAdapter1> m_Adapter;
    ComPtr<IDXGIFactory4> m_Factory;
    ComPtr<ID3D11RenderTargetView> m_RenderTargetView;

    D3D11_VIEWPORT m_Viewport;
    D3D11_RECT m_ScissorRect;
};

#endif // !HELLO_HPP
