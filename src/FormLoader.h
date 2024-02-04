#pragma once

#include "Utility.h"
#include "Settings.h"

#undef AddForm

class FormLoader {
public:
    const std::string_view pluginSkyrim = "Skyrim.esm";
    const std::string_view pluginDawnguard = "Dawnguard.esm";
    const std::string_view pluginUndeath = "Undeath.esp";

    static FormLoader* GetSingleton() {
        static FormLoader formLoader;
        return &formLoader;
    }

    void LoadAllForms() {
        logger::info("Loading all forms.");

        const auto dataHandler = RE::TESDataHandler::GetSingleton();

        LoadMiscForms(dataHandler);
        logger::info("All forms are loaded.");

        LoadCompatibilityForms(dataHandler);
        logger::info("Compatibility forms are loaded.");
    }

    void LoadMiscForms([[maybe_unused]] RE::TESDataHandler* dataHandler) {
        auto utility = Utility::GetSingleton();
        auto settings = Settings::GetSingleton();

        // Health Effect Names
        settings->HealthRestoreName = dataHandler->LookupForm(RE::FormID(0x3EB15), pluginSkyrim)->As<RE::EffectSetting>()->GetFullName();
        settings->HealthRegenName = dataHandler->LookupForm(RE::FormID(0x3EB06), pluginSkyrim)->As<RE::EffectSetting>()->GetFullName();
        settings->HealthFortifyName = dataHandler->LookupForm(RE::FormID(0x3EAF3), pluginSkyrim)->As<RE::EffectSetting>()->GetFullName();

        // Magicka Effect Names
        settings->MagickaRestoreName = dataHandler->LookupForm(RE::FormID(0x3EB17), pluginSkyrim)->As<RE::EffectSetting>()->GetFullName();
        settings->MagickaRegenName = dataHandler->LookupForm(RE::FormID(0x3EB07), pluginSkyrim)->As<RE::EffectSetting>()->GetFullName();
        settings->MagickaFortifyName = dataHandler->LookupForm(RE::FormID(0x3EAF8), pluginSkyrim)->As<RE::EffectSetting>()->GetFullName();

        // Stamina Effect Names
        settings->StaminaRestoreName = dataHandler->LookupForm(RE::FormID(0x3EB16), pluginSkyrim)->As<RE::EffectSetting>()->GetFullName();
        settings->StaminaRegenName = dataHandler->LookupForm(RE::FormID(0x3EB08), pluginSkyrim)->As<RE::EffectSetting>()->GetFullName();
        settings->StaminaFortifyName = dataHandler->LookupForm(RE::FormID(0x3EAF9), pluginSkyrim)->As<RE::EffectSetting>()->GetFullName();

        // Additional Effect Names
        settings->CureDiseaseName = dataHandler->LookupForm(RE::FormID(0xAE722), pluginSkyrim)->As<RE::EffectSetting>()->GetFullName();
        settings->CurePoisonName = dataHandler->LookupForm(RE::FormID(0x109ADD), pluginSkyrim)->As<RE::EffectSetting>()->GetFullName();
         
        // Effect Keywords
        utility->positiveKeyword = dataHandler->LookupForm(RE::FormID(0xF8A4E), pluginSkyrim)->As<RE::BGSKeyword>();
        utility->negativeKeyword = dataHandler->LookupForm(RE::FormID(0x42509), pluginSkyrim)->As<RE::BGSKeyword>();

        // Transformation Races
        utility->raceWerewolf = dataHandler->LookupForm(RE::FormID(0x0CDD84), pluginSkyrim)->As<RE::TESRace>();
        utility->raceVampireLord = dataHandler->LookupForm(RE::FormID(0x00283A), pluginDawnguard)->As<RE::TESRace>();

        // Quests
        utility->BrawlQuest = dataHandler->LookupForm(RE::FormID(0x47AE6), pluginSkyrim)->As<RE::TESQuest>();
    }

    void LoadCompatibilityForms(RE::TESDataHandler* dataHandler) {
        auto util = Utility::GetSingleton();

        if (dataHandler->LookupLoadedModByName(pluginUndeath)) {
            auto lichPerk = dataHandler->LookupForm(RE::FormID(0x3326D5), pluginUndeath);
            if (lichPerk) {
                util->Undeath_LichPerk = lichPerk->As<RE::BGSPerk>();
            }
        }
    }

    // Cache commonly called addresses to avoid address lib overhead
    void CacheGameAddresses() {
        auto utility = Utility::GetSingleton();

        // Cache player singleton address
        utility->PlayerSingletonAddress = RELOCATION_ID(517014, 403521).address();
        utility->UISingletonAddress = RELOCATION_ID(514178, 400327).address();
        utility->MenuControlsSingletonAddress = RELOCATION_ID(515124, 401263).address();
    }
};