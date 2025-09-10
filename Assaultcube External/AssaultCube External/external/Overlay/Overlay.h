#pragma once
#include <Windows.h>
#include <dwmapi.h>
#include <d3d11.h>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../Visuals/Radar.h"
#include "../Visuals/PlayerESP.h"
#include "../Memory/GetValues.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Overlay {
    public:
        Overlay();
        ~Overlay();

        bool Initialize(HWND get_hwndGame, const RECT& target_rect, int cmd_show);
        void Shutdown();               // Clean up resources
        void RenderOverlay(const GetValues::MemoryValues& memValues);
        static LRESULT CALLBACK window_procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        void BeginRender();
        void EndRender();
        HWND GetWindow() const { return hwndOverlay; }

    private:
        ID3D11Device* device{ nullptr };
        ID3D11DeviceContext* device_context{ nullptr };
        IDXGISwapChain* swap_chain{ nullptr };
        ID3D11RenderTargetView* render_target_view{ nullptr };
        WNDCLASSEXW wndclass = {};
        bool initialized = false;
        const float clear_color_with_alpha[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // Transparent background

        bool CreateOverlayWindow(const RECT& target_rect, int cmd_show);
        bool SetupDirectX();
        void SetupImGui();
        void ReleaseResources();
        void ResizeBuffers(UINT width, UINT height);

        static Overlay* instance;
        void HandleMessages(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
        HWND hwndOverlay = nullptr;
        HWND hwndGame = nullptr;

        Mathematics random;
};
