#include "Overlay.h"

Overlay* Overlay::instance = nullptr;

Overlay::Overlay() {
    instance = this; // Set the instance pointer
}

Overlay::~Overlay() {
    //Shutdown(); // Ensure resources are released
}

bool Overlay::Initialize(HWND get_hwndGame, const RECT& target_rect, int cmd_show) {
    hwndGame = get_hwndGame;

    if (!CreateOverlayWindow(target_rect, cmd_show) || !SetupDirectX()) {
        return false;
    }

    SetupImGui();

    initialized = true;
    return true;
}

void Overlay::BeginRender() {
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Clear the back buffer
    device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
    const float clear_color_with_alpha[4] = { 0.f, 0.f, 0.f, 0.f };
    device_context->ClearRenderTargetView(render_target_view, clear_color_with_alpha);
}

void Overlay::EndRender() {

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Present(1, 0) enables VSync. This caps overlay's FPS to monitor's
    // refresh rate, freeing up CPU/GPU for the game and fixing low FPS issues.
    swap_chain->Present(1U, 0U);
}

void Overlay::Shutdown() {
    if (!initialized) return;

    if (ImGui::GetCurrentContext()) {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    ReleaseResources();
    if (hwndOverlay) {
        DestroyWindow(hwndOverlay);
        UnregisterClassW(wndclass.lpszClassName, wndclass.hInstance);
    }

    initialized = false;
}

bool Overlay::CreateOverlayWindow(const RECT& target_rect, int cmd_show) {

    int width = target_rect.right - target_rect.left;
    int height = target_rect.bottom - target_rect.top;

    static std::wstring globalClassName = random.generateRandomWideString();
    static std::wstring globalWindowName = random.generateRandomWideString();

    wndclass.cbSize = sizeof(WNDCLASSEXW);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = window_procedure;
    wndclass.hInstance = GetModuleHandle(nullptr);
    wndclass.lpszClassName = globalClassName.c_str();

    if (!RegisterClassExW(&wndclass)) {
        return false;
    }



    hwndOverlay = CreateWindowExW(
        WS_EX_TRANSPARENT | WS_EX_LAYERED,
        wndclass.lpszClassName,
        globalWindowName.c_str(),
        WS_POPUP,
        target_rect.left,
        target_rect.top, 
        width, 
        height,
        hwndGame, //make game parentwindow of overlay
        nullptr,
        wndclass.hInstance,
        nullptr);

    if (!hwndOverlay) {
        return false;
    }

    SetLayeredWindowAttributes(hwndOverlay, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);
    {
        RECT client_area{};
        GetClientRect(hwndOverlay, &client_area);

        RECT window_area{};
        GetWindowRect(hwndOverlay, &window_area);

        POINT diff{};
        ClientToScreen(hwndOverlay, &diff);

        const MARGINS margins{
            window_area.left + (diff.x - window_area.left),
            window_area.top + (diff.y - window_area.top),
            client_area.right,
            client_area.bottom
        };
        //MARGINS margins = { -1 };

        DwmExtendFrameIntoClientArea(hwndOverlay, &margins);
    }

    ShowWindow(hwndOverlay, cmd_show);
    //ShowWindow(window, SW_SHOWDEFAULT);
    UpdateWindow(hwndOverlay);

    return true;
}

bool Overlay::SetupDirectX() {
    DXGI_SWAP_CHAIN_DESC swapdesc{};
    //ZeroMemory(&swapdesc, sizeof(swapdesc));
    swapdesc.BufferDesc.RefreshRate.Numerator = 0U; //60U fixed refreshrate 0U let system decide
    swapdesc.BufferDesc.RefreshRate.Denominator = 0U; //1U
    swapdesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapdesc.SampleDesc.Count = 1U;
    swapdesc.SampleDesc.Quality = 0U;
    swapdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapdesc.BufferCount = 2U;
    swapdesc.OutputWindow = hwndOverlay;
    swapdesc.Windowed = TRUE;
    swapdesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

    constexpr D3D_FEATURE_LEVEL levels[2] = {
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_11_0
    };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0U,
        levels,
        2U, //feature level
        D3D11_SDK_VERSION,
        &swapdesc,
        &swap_chain,
        &device,
        nullptr,
        &device_context);

    if (FAILED(hr)) {
        return false;
    }

    ID3D11Texture2D* back_buffer = nullptr;
    swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
    if (back_buffer) {
        hr = device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
        back_buffer->Release();
    }

    return SUCCEEDED(hr);
}

void Overlay::SetupImGui() {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    RECT overlay_rect;
    GetClientRect(this->hwndOverlay, &overlay_rect);
    io.DisplaySize.x = static_cast<float>(overlay_rect.right - overlay_rect.left);
    io.DisplaySize.y = static_cast<float>(overlay_rect.bottom - overlay_rect.top);
    io.IniFilename = nullptr; // Disables imgui.ini file creation
    io.LogFilename = nullptr;

    ImGui_ImplWin32_Init(hwndOverlay);
    ImGui_ImplDX11_Init(device, device_context);
}

void Overlay::ReleaseResources() {
    if (swap_chain) {
        swap_chain->Release();
    }
    if (device_context) {
        device_context->Release();
    }
    if (device) {
        device->Release();
    }
    if (render_target_view) {
        render_target_view->Release();
    }
}

LRESULT CALLBACK Overlay::window_procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
//LRESULT WINAPI Overlay::window_procedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){

    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) 
        return true;

    if (instance) {
        instance->HandleMessages(hWnd, msg, wParam, lParam);
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void Overlay::HandleMessages(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_SIZE:
        if (device && wParam != SIZE_MINIMIZED) {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);

            ResizeBuffers(width, height);
        }
        return; 

    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return;
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return;
    }
}

void Overlay::ResizeBuffers(UINT width, UINT height) {

    if (render_target_view) {
        render_target_view->Release();
        render_target_view = nullptr;
    }

    HRESULT hr = swap_chain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) {
        return;
    }

    ID3D11Texture2D* back_buffer = nullptr;
    swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
    if (!back_buffer) {
        return;
    }

    device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
    back_buffer->Release();

    ImGui::GetIO().DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
}
