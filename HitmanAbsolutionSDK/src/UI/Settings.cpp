#include <filesystem>
#include <format>

#include <imgui.h>

#include <IconsMaterialDesign.h>

#define MINI_CASE_SENSITIVE

#include <ini.h>

#include <Glacier/Engine/ZApplicationEngineWin32.h>

#include <UI/Settings.h>
#include <SDK.h>
#include <Global.h>

Settings::Settings()
{
    isOpen = false;
    readEngineIni = false;
    patchResources = false;
    pauseOnFocusLoss = false;
    minimizeOnFocusLoss = false;

    GetValueFromIniFile("Settings", "ReadEngineIni", readEngineIni);
    GetValueFromIniFile("Settings", "PatchResources", patchResources);
    GetValueFromIniFile("Settings", "PauseOnFocusLoss", pauseOnFocusLoss);
    GetValueFromIniFile("Settings", "MinimizeOnFocusLoss", minimizeOnFocusLoss);
}

void Settings::Draw(const bool hasFocus)
{
    if (!hasFocus || !isOpen)
    {
        return;
    }

    ImGui::PushFont(SDK::GetInstance().GetBoldFont());
    ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);

    const bool isWindowVisible = ImGui::Begin(ICON_MD_SETTINGS " Settings", &isOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

    ImGui::PushFont(SDK::GetInstance().GetRegularFont());

    if (isWindowVisible)
    {
        if (ImGui::Checkbox("Read engine.ini", &readEngineIni))
        {
            UpdateIniFile("ReadEngineIni", readEngineIni);
        }

        if (ImGui::Checkbox("Patch Resources", &patchResources))
        {
            UpdateIniFile("PatchResources", patchResources);
        }

        if (ImGui::Checkbox("Pause On Focus Loss", &pauseOnFocusLoss))
        {
            UpdateIniFile("PauseOnFocusLoss", pauseOnFocusLoss);

            const char* value = pauseOnFocusLoss ? "true" : "false";

            ZApplicationEngineWin32::GetInstance()->SetOption("PauseOnFocusLoss", value);
        }

        if (ImGui::Checkbox("Minimize On Focus Loss", &minimizeOnFocusLoss))
        {
            UpdateIniFile("MinimizeOnFocusLoss", minimizeOnFocusLoss);

            const char* value = minimizeOnFocusLoss ? "true" : "false";

            ZApplicationEngineWin32::GetInstance()->SetOption("NO_MINIMIZE_FOCUSLOSS", value);
        }
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void Settings::Show()
{
    isOpen = true;
}

const bool Settings::ReadEngineIni() const
{
    return readEngineIni;
}

const bool Settings::PatchResources() const
{
    return patchResources;
}

const bool Settings::PauseOnFocusLoss() const
{
    return pauseOnFocusLoss;
}

const bool Settings::MinimizeOnFocusLoss() const
{
    return minimizeOnFocusLoss;
}

void Settings::SetPauseOnFocusLoss(const bool pauseOnFocusLoss)
{
    this->pauseOnFocusLoss = pauseOnFocusLoss;
}

void Settings::SetMinimizeOnFocusLoss(const bool minimizeOnFocusLoss)
{
    this->minimizeOnFocusLoss = minimizeOnFocusLoss;
}

const bool Settings::IniFileHasKey(const std::string& section, const std::string& key)
{
    const std::filesystem::path iniFilePath = std::format("{}\\HitmanAbsolutionSDK.ini", std::filesystem::current_path().string());

    if (!std::filesystem::exists(iniFilePath))
    {
        return false;
    }

    const mINI::INIFile iniFile = mINI::INIFile(iniFilePath.string());
    mINI::INIStructure iniStructure;

    iniFile.read(iniStructure);

    if (iniStructure.has(section) && iniStructure[section].has(key))
    {
        return true;
    }

    return false;
}

void Settings::GetValueFromIniFile(const std::string& section, const std::string& key, bool& outValue)
{
    const std::filesystem::path iniFilePath = std::format("{}\\HitmanAbsolutionSDK.ini", std::filesystem::current_path().string());

    if (!std::filesystem::exists(iniFilePath))
    {
        return;
    }

    const mINI::INIFile iniFile = mINI::INIFile(iniFilePath.string());
    mINI::INIStructure iniStructure;

    iniFile.read(iniStructure);

    if (iniStructure.has(section) && iniStructure[section].has(key))
    {
        std::string value = iniStructure[section][key];

        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c)
        {
            return std::tolower(c);
        });

        outValue = value == "true" || value == "1";
    }
}

void Settings::UpdateIniFile(const std::string& key, const bool value)
{
    const std::filesystem::path iniFilePath = std::format("{}\\HitmanAbsolutionSDK.ini", std::filesystem::current_path().string());
    const mINI::INIFile iniFile = mINI::INIFile(iniFilePath.string());
    mINI::INIStructure iniStructure;

    if (std::filesystem::exists(iniFilePath))
    {
        mINI::INIStructure oldIni;

        iniFile.read(oldIni);
    }

    iniStructure["Settings"][key] = value ? "true" : "false";

    iniFile.generate(iniStructure, true);
}
