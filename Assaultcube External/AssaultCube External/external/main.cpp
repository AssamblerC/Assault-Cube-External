#include <Windows.h>
#include "Overlay/Overlay.h"
#include "Memory/GetValues.h"
#include "Overlay/Menu.h"
#include "Aim/Aimbot.h"
#include "Utility/ExternMath.h"
#include "Aim/Triggerbot.h"
#include "Visuals/ItemESP.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmdShow) {

    GetValues handle;
    handle.InitializeHandle();

    if (handle.hwndGame == 0) {
        return EXIT_FAILURE;
    }

    RECT game_client_rect;
    GetClientRect(handle.hwndGame, &game_client_rect);

    POINT top_left = { game_client_rect.left, game_client_rect.top };
    ClientToScreen(handle.hwndGame, &top_left);

    RECT overlay_initial_rect = {
        top_left.x,
        top_left.y,
        top_left.x + (game_client_rect.right - game_client_rect.left),
        top_left.y + (game_client_rect.bottom - game_client_rect.top)
    };


    RECT initial_rect;
    Overlay overlay;
    PlayerESP playeresp;
    ItemESP itemEsp;
    Radar radar;
    Aimbot aimbot;
    Triggerbot triggerbot;
    Mathematics math;
    Menu menu;

    if (!overlay.Initialize(handle.hwndGame, initial_rect, SW_SHOWDEFAULT) || handle.hwndGame == 0 || !GetWindowRect(handle.hwndGame, &initial_rect)) {
        handle.Cleanup();
        overlay.Shutdown();
        exit(EXIT_FAILURE);
    }

    ImGuiStyle& style = ImGui::GetStyle();
    menu.ApplyStyle(style);

    bool showMenu = false;
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            GetValues::MemoryValues memoryValues = handle.GetMemoryValues(overlay.GetWindow());

            if (!memoryValues.gameOpen) {
                break;
            }

            if (GetAsyncKeyState(VK_DELETE) & 1) {
                showMenu = !showMenu;

                SetForegroundWindow(showMenu ? overlay.GetWindow() : handle.hwndGame);
            }

            LONG_PTR currentStyle = GetWindowLongPtr(overlay.GetWindow(), GWL_EXSTYLE);
            LONG_PTR newStyle;

            if (showMenu) {
                newStyle = currentStyle & ~WS_EX_TRANSPARENT;

                ImGui::GetIO().MouseDrawCursor = true;
                ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
            }
            else {
                newStyle = currentStyle | WS_EX_TRANSPARENT;

                ImGui::GetIO().MouseDrawCursor = false;
                ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
            }
            if (newStyle != currentStyle) {
                SetWindowLongPtr(overlay.GetWindow(), GWL_EXSTYLE, newStyle);
            }

            overlay.BeginRender();

            if (showMenu) {
                menu.Draw();
            }
            else {
                if (menu.GetOptions().DrawPlayerESP) {
                    playeresp.drawPlayerESP(memoryValues, menu.GetOptions(), math);
                }
                if (menu.GetOptions().DrawItemESP) {
                    itemEsp.drawItemESP(memoryValues, menu.GetOptions(), math);
                }
                if (menu.GetOptions().DrawRadar) {
                    radar.drawRadar(memoryValues, menu.GetOptions(), math);
                }
                if (GetForegroundWindow() == handle.hwndGame) {
                    aimbot.aimUpdate(memoryValues, math, menu.GetOptions());
                    triggerbot.triggerUpdate(memoryValues, menu.GetOptions(), math);
                }
            }

            overlay.EndRender();

            if (GetForegroundWindow() != handle.hwndGame && !showMenu) {
                Sleep(250); //reduce CPU usage if game is not in focus
            }
            else {
                Sleep(1);
            }
        }
    }

    handle.Cleanup();
    overlay.Shutdown();

    return EXIT_SUCCESS;
};