#include "dx11.h"
#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>

#include <gui/gui.h>
#include <gui/mainwindow.h>
#include <cheat/hooks.h>
#include <extern/imgui/imgui.h>
#include <extern/imgui/imgui_impl_win32.h>
#include <extern/imgui/imgui_impl_dx11.h>
#include <drawing/drawing.h>
#include <cheat/core.h>

#pragma comment(lib, "d3d11.lib")

namespace GUI {
namespace DX11 {

// --- DX11 Data -----------------------------------------------------

IDXGISwapChain*         g_SwapChain = nullptr;
ID3D11Device*           g_Device = nullptr;
ID3D11DeviceContext*    g_DeviceContext = nullptr;
ID3D11RenderTargetView* g_RenderTargetView = nullptr;
HWND                    g_Window = nullptr;
void*                   g_PresentFunc = nullptr;
void*                   g_ResizeBuffersFunc = nullptr;
void*                   g_WndProc = nullptr;
bool                    g_PendingShutdown = false;
bool                    g_Hooked = false;

// --- Hook Functions ------------------------------------------------

typedef HRESULT(__stdcall* t_Present)(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags);
inline t_Present o_Present = nullptr;
static HRESULT __stdcall h_Present(IDXGISwapChain* SwapChain, UINT SyncInterval, UINT Flags) {
    std::lock_guard<std::recursive_mutex> lock(GUI::g_WndProcMutex);
    std::lock_guard<std::mutex>           lock2(Core::g_GameRenderThreadLock);

    if (!GUI::g_SetupImGui) {
        GUI::g_SetupImGui = true;

        if (SUCCEEDED(SwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_Device))) {
            g_Device->GetImmediateContext(&g_DeviceContext);

            DXGI_SWAP_CHAIN_DESC Desc = {};
            SwapChain->GetDesc(&Desc);
            g_Window = Desc.OutputWindow;

            ID3D11Texture2D* BackBuffer = nullptr;
            SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);
            g_Device->CreateRenderTargetView(BackBuffer, NULL, &g_RenderTargetView);
            BackBuffer->Release();

            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

            Drawing::Init();

            ImGui_ImplWin32_Init(g_Window);
            ImGui_ImplDX11_Init(g_Device, g_DeviceContext);

            g_WndProc = (void*)GetWindowLongPtrW(g_Window, GWLP_WNDPROC);
            Hooks::CreateHook(g_WndProc, &GUI::h_WndProc, (void**)&GUI::o_WndProc);
            Hooks::EnableHook(g_WndProc);
        }
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    Core::TickRenderThread();

    ImGui::EndFrame();
    ImGui::Render();

    g_DeviceContext->OMSetRenderTargets(1, &g_RenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    if (g_PendingShutdown) {
        GUI::g_SetupImGui = false;

        Hooks::RemoveHook(g_PresentFunc);
        Hooks::RemoveHook(g_ResizeBuffersFunc);
        Hooks::RemoveHook(g_WndProc);

        ImGui_ImplDX11_CustomShutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        g_DeviceContext = nullptr;
        g_Device = nullptr;
        g_SwapChain = nullptr;

        g_PendingShutdown = false;
        return 0;
    }

    return o_Present(SwapChain, SyncInterval, Flags);
}

using t_ResizeBuffers = HRESULT (*)(IDXGISwapChain* pThis, UINT BufferCount, UINT Width, UINT Height,
                                    DXGI_FORMAT NewFormat, UINT SwapChainFlags);
inline t_ResizeBuffers o_ResizeBuffers = nullptr;
static HRESULT __stdcall h_ResizeBuffers(IDXGISwapChain* pThis, UINT BufferCount, UINT Width, UINT Height,
                                  DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
    if (g_RenderTargetView) {
        g_DeviceContext->OMSetRenderTargets(0, 0, 0);
        g_RenderTargetView->Release();
    }

    HRESULT Result = o_ResizeBuffers(pThis, BufferCount, Width, Height, NewFormat, SwapChainFlags);

    ID3D11Texture2D* Buffer = nullptr;
    pThis->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&Buffer);

    g_Device->CreateRenderTargetView(Buffer, NULL, &g_RenderTargetView);
    Buffer->Release();

    g_DeviceContext->OMSetRenderTargets(1, &g_RenderTargetView, NULL);

    D3D11_VIEWPORT vp = {};
    vp.Width = static_cast<FLOAT>(Width);
    vp.Height = static_cast<FLOAT>(Height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_DeviceContext->RSSetViewports(1, &vp);

    return Result;
}

// --- Initialization ------------------------------------------------

bool Init() {
    HMODULE D3D11 = GetModuleHandleA("d3d11.dll");
    if (!D3D11)
        return false;

    D3D_FEATURE_LEVEL       FeatureLevel = D3D_FEATURE_LEVEL_11_0;
    const D3D_FEATURE_LEVEL FeatureLevels[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1};

    DXGI_RATIONAL RefreshRate = {};
    RefreshRate.Numerator = 60;
    RefreshRate.Denominator = 1;

    DXGI_MODE_DESC BufferDesc = {};
    BufferDesc.Width = 100;
    BufferDesc.Height = 100;
    BufferDesc.RefreshRate = RefreshRate;
    BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SAMPLE_DESC SampleDesc = {};
    SampleDesc.Count = 1;
    SampleDesc.Quality = 0;

    DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
    SwapChainDesc.BufferDesc = BufferDesc;
    SwapChainDesc.SampleDesc = SampleDesc;
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDesc.BufferCount = 1;
    SwapChainDesc.OutputWindow = GetForegroundWindow();
    SwapChainDesc.Windowed = 1;
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    ID3D11Device*        Device = nullptr;
    ID3D11DeviceContext* DeviceContext = nullptr;
    IDXGISwapChain*      SwapChain = nullptr;

    HRESULT Result = ((HRESULT (*)(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT, const D3D_FEATURE_LEVEL*, UINT, UINT,
                                   const DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D11Device**, D3D_FEATURE_LEVEL*,
                                   ID3D11DeviceContext**))D3D11CreateDeviceAndSwapChain)(
        NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, FeatureLevels, 1, D3D11_SDK_VERSION, &SwapChainDesc, &SwapChain,
        &Device, &FeatureLevel, &DeviceContext);
    if (FAILED(Result))
        return false;

    if (!SwapChain || !*(void***)SwapChain)
        return false;

    void** VFT = *(void***)SwapChain;
    g_PresentFunc = VFT[8];
    g_ResizeBuffersFunc = VFT[13];

    SwapChain->Release();
    Device->Release();
    DeviceContext->Release();

    g_Hooked = Hooks::CreateHook(g_PresentFunc, &h_Present, reinterpret_cast<void**>(&o_Present)) &&
             Hooks::EnableHook(g_PresentFunc) &&
             Hooks::CreateHook(g_ResizeBuffersFunc, &h_ResizeBuffers, reinterpret_cast<void**>(&o_ResizeBuffers)) &&
             Hooks::EnableHook(g_ResizeBuffersFunc);
    return g_Hooked;
}

void Destroy() {
    // Spin until h_Present has finished...
    if (g_Hooked) {
        g_PendingShutdown = true;
        while (g_PendingShutdown) {
            Sleep(100);
        }
    }
}

} // namespace DX11
} // namespace GUI
