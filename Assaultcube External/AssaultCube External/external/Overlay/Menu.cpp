#include "Menu.h"
#include "../Utility/Obfus.h"

Menu::Menu() {
    //ApplyStyle();
}

Menu::MenuOptions Menu::GetOptions() {
    return options;
}

void Menu::Draw() {
    ImGui::SetNextWindowSize(ImVec2(650, 450), ImGuiCond_FirstUseEver);
    ImGui::Begin("Read Only External", &menuIsOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

    // --- LEFT SIDEBAR ---
    ImGui::BeginChild("Sidebar", ImVec2(150, 0), false);
    {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        ImGui::Text(GetObfusString().c_str());
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

        // --- Main Tabs ---
        bool legitBotActive = (activeMainTab == MainTabs::LegitBot);
        if (legitBotActive) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
        if (ImGui::Button("LegitBot", ImVec2(120, 35))) { activeMainTab = MainTabs::LegitBot; }
        if (legitBotActive) ImGui::PopStyleColor();

        bool visualsActive = (activeMainTab == MainTabs::Visuals);
        if (visualsActive) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
        if (ImGui::Button("Visuals", ImVec2(120, 35))) { activeMainTab = MainTabs::Visuals; }
        if (visualsActive) ImGui::PopStyleColor();

        // --- Changed Misc to Radar ---
        bool radarActive = (activeMainTab == MainTabs::Radar);
        if (radarActive) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
        if (ImGui::Button("Radar", ImVec2(120, 35))) { activeMainTab = MainTabs::Radar; }
        if (radarActive) ImGui::PopStyleColor();

    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("Content", ImVec2(0, 0), false);
    {
        // --- LegitBot Tab ---
        if (activeMainTab == MainTabs::LegitBot) {
            // Sub-tabs
            if (ImGui::Button("AimAssist")) { activeLegitSubTab = LegitSubTabs::AimAssist; }
            ImGui::SameLine();
            if (ImGui::Button("TriggerBot")) { activeLegitSubTab = LegitSubTabs::TriggerBot; }
            ImGui::Separator();

            // Content for Sub-tabs
            if (activeLegitSubTab == LegitSubTabs::AimAssist) {
                ImGui::BeginChild("AimAssistPanel", ImVec2(0, 0), true);
                {
                    ImGui::Checkbox("Aimbot", &options.AimbotEnabled);
                    ImGui::Separator();
                    if (!options.AimbotEnabled) ImGui::BeginDisabled();
                    ImGui::Checkbox("Only target Visible", &options.AimbotVisCheck);
                    ImGui::Checkbox("Draw FOV Circle", &options.AimbotDrawFov);
                    ImGui::SliderFloat("FOV", &options.AimbotFov, 2.0f, 800.0f, "%.0f");
                    ImGui::SliderFloat("Speed", &options.AimbotSpeed, 10.0f, 50.0f, "%.1f");
                    ImGui::Separator();
                    ImGui::Checkbox("Deadzone Speed Reset", &options.AimbotDeadzoneReset);
                    if (!options.AimbotDeadzoneReset) ImGui::BeginDisabled();
                    ImGui::SliderFloat("Deadzone Size", &options.AimbotDeadzoneSize, 0.1f, 0.5f, "%.2f");
                    ImGui::SliderFloat("Reset Speed", &options.AimbotDeadzoneSpeed, 1.0f, 50.0f, "%.1f");
                    ImGui::SliderInt("Reset Duration (ms)", &options.AimbotDeadzoneDuration, 100, 2000, "%d ms");
                    if (!options.AimbotDeadzoneReset) ImGui::EndDisabled();
                    if (!options.AimbotEnabled) ImGui::EndDisabled();
                }
                ImGui::EndChild();
            }
            else if (activeLegitSubTab == LegitSubTabs::TriggerBot) {
                ImGui::BeginChild("TriggerBotPanel", ImVec2(0, 0), true);
                {
                    ImGui::Checkbox("Triggerbot", &options.TriggerbotEnabled);
                    ImGui::Separator();
                    if (!options.TriggerbotEnabled) ImGui::BeginDisabled();
                    ImGui::SliderFloat("Trigger FOV", &options.TriggerbotFov, 5.0f, 20.0f, "%.1f ");
                    ImGui::Checkbox("Draw FOV", &options.TriggerbotFovDraw);
                    if (!options.TriggerbotEnabled) ImGui::EndDisabled();
                }
                ImGui::EndChild();
            }
        }
        // --- Visuals Tab ---
        else if (activeMainTab == MainTabs::Visuals) {
            if (ImGui::Button("Player ESP")) { activeVisualsSubTab = VisualsSubTabs::PlayerESP; }
            ImGui::SameLine();
            if (ImGui::Button("Item ESP")) { activeVisualsSubTab = VisualsSubTabs::ItemESP; }
            ImGui::Separator();

            // Content for Visuals Sub-tabs
            if (activeVisualsSubTab == VisualsSubTabs::PlayerESP) {
                ImGui::BeginChild("PlayerESPPanel", ImVec2(0, 0), true);
                {
                    ImGui::Checkbox("ESP", &options.DrawPlayerESP);
                    ImGui::Separator();
                    if (!options.DrawPlayerESP) ImGui::BeginDisabled();
                    ImGui::Checkbox("Team", &options.TeamESP);
                    ImGui::Checkbox("Enemy", &options.EnemyESP);
                    ImGui::Checkbox("Only Show Visible", &options.VisibleCheck);
                    ImGui::Checkbox("Health", &options.HealthESP);
                    ImGui::Checkbox("Armor", &options.ArmorESP);
                    ImGui::Checkbox("Distance", &options.PlayerDistanceESP);
                    ImGui::Checkbox("Username", &options.PlayerUsernameESP);
                    if (!options.DrawPlayerESP) ImGui::EndDisabled();
                }
                ImGui::EndChild();
            }
            else if (activeVisualsSubTab == VisualsSubTabs::ItemESP) {
                ImGui::BeginChild("ItemESPPanel", ImVec2(0, 0), true);
                {
                    ImGui::Checkbox("Item ESP", &options.DrawItemESP);
                    ImGui::Separator();
                    if (!options.DrawItemESP) ImGui::BeginDisabled();
                    ImGui::Checkbox("Draw Box", &options.ItemBoxESP);
                    ImGui::Checkbox("Draw Distance", &options.ItemDistanceESP);

                    if (!options.ItemDistanceESP) ImGui::BeginDisabled();
                    ImGui::SliderFloat("Max Distance", &options.ItemMaxDistance, 10.0f, 500.0f, "%.0f");
                    if (!options.ItemDistanceESP) ImGui::EndDisabled();

                    ImGui::SliderFloat("Opacity", &options.ItemOpacity, 0.1f, 1.0f, "%.2f");
                    ImGui::Separator();
                    ImGui::Text("Show Items:");
                    ImGui::Checkbox("Pistol Ammo", &options.ShowPistolAmmo); ImGui::SameLine(150);
                    ImGui::Checkbox("Weapon Ammo", &options.ShowWeaponAmmo);
                    ImGui::Checkbox("Grenades", &options.ShowGrenades); ImGui::SameLine(150);
                    ImGui::Checkbox("Health", &options.ShowHealth);
                    ImGui::Checkbox("Helmet", &options.ShowHelmet); ImGui::SameLine(150);
                    ImGui::Checkbox("Kevlar", &options.ShowKevlar);
                    ImGui::Checkbox("Akimbo", &options.ShowAkimbo); ImGui::SameLine(150);
                    ImGui::Checkbox("Flag", &options.ShowFlag);
                    if (!options.DrawItemESP) ImGui::EndDisabled();
                }
                ImGui::EndChild();
            }
        }
        // --- Radar Tab ---
        else if (activeMainTab == MainTabs::Radar) {
            ImGui::BeginChild("RadarPanel", ImVec2(0, 0), true);
            {
                ImGui::Checkbox("Radar", &options.DrawRadar);
                ImGui::Separator();
            }
            ImGui::EndChild();
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void Menu::ApplyStyle(ImGuiStyle& style) {

    ImVec4 mint_green = ImVec4(102.0f / 255.0f, 255.0f / 255.0f, 153.0f / 255.0f, 1.0f);
    ImVec4 mint_green_trans = ImVec4(102.0f / 255.0f, 255.0f / 255.0f, 153.0f / 255.0f, 0.8f);

    style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.11f, 0.12f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.22f, 0.23f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_CheckMark] = mint_green;
    style.Colors[ImGuiCol_SliderGrab] = mint_green;
    style.Colors[ImGuiCol_SliderGrabActive] = mint_green_trans;
    style.Colors[ImGuiCol_Button] = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = mint_green;
    style.Colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.35f, 0.36f, 1.00f);
    style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];

    style.WindowPadding = ImVec2(8.00f, 8.00f);
    style.FramePadding = ImVec2(5.00f, 2.00f);
    style.ItemSpacing = ImVec2(6.00f, 6.00f);
    style.WindowRounding = 6.0f;
    style.ChildRounding = 6.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 6.0f;
    style.GrabRounding = 6.0f;
}