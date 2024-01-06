#pragma once

#include "Utility.h"
#include "Settings.h"

#undef AddForm

class FormLoader {
public:
    const std::string_view swiftPluginHame = "SwiftPotionNG.esp";
    const std::string_view skyrimPluginName = "Skyrim.esm";
    const std::string_view updatePluginName = "Update.esm";
    const std::string_view dgPluginName = "Dawnguard.esm";
    const std::string_view hfPluginName = "HearthFires.esm";
    const std::string_view undeathPluginName = "Undeath.esp";

    static FormLoader* GetSingleton() {
        static FormLoader formLoader;
        return &formLoader;
    }

    void LoadAllForms() {
        logger::info("Loading all forms.");

        const auto dataHandler = RE::TESDataHandler::GetSingleton();

        if (!dataHandler->LookupLoadedLightModByName(swiftPluginHame)) return;

        LoadMiscForms(dataHandler);
        logger::info("All forms are loaded.");

        LoadCompatibilityForms(dataHandler);
        logger::info("Compatibility forms are loaded.");
    }

    void LoadMiscForms([[maybe_unused]] RE::TESDataHandler* dataHandler) {
        auto utility = Utility::GetSingleton();
        auto settings = Settings::GetSingleton();

        // Health Effect Names
        settings->HealthRestoreName = dataHandler->LookupForm(RE::FormID(0x3EB15), skyrimPluginName)->As<RE::EffectSetting>()->GetFullName();
        settings->HealthRegenName = dataHandler->LookupForm(RE::FormID(0x3EB06), skyrimPluginName)->As<RE::EffectSetting>()->GetFullName();
        settings->HealthFortifyName = dataHandler->LookupForm(RE::FormID(0x3EAF3), skyrimPluginName)->As<RE::EffectSetting>()->GetFullName();

        // Magicka Effect Names
        settings->MagickaRestoreName = dataHandler->LookupForm(RE::FormID(0x3EB17), skyrimPluginName)->As<RE::EffectSetting>()->GetFullName();
        settings->MagickaRegenName = dataHandler->LookupForm(RE::FormID(0x3EB07), skyrimPluginName)->As<RE::EffectSetting>()->GetFullName();
        settings->MagickaFortifyName = dataHandler->LookupForm(RE::FormID(0x3EAF8), skyrimPluginName)->As<RE::EffectSetting>()->GetFullName();

        // Stamina Effect Names
        settings->StaminaRestoreName = dataHandler->LookupForm(RE::FormID(0x3EB16), skyrimPluginName)->As<RE::EffectSetting>()->GetFullName();
        settings->StaminaRegenName = dataHandler->LookupForm(RE::FormID(0x3EB08), skyrimPluginName)->As<RE::EffectSetting>()->GetFullName();
        settings->StaminaFortifyName = dataHandler->LookupForm(RE::FormID(0x3EAF9), skyrimPluginName)->As<RE::EffectSetting>()->GetFullName();

        // Additional Effect Names
        settings->CureDiseaseName = dataHandler->LookupForm(RE::FormID(0xAE722), skyrimPluginName)->As<RE::EffectSetting>()->GetFullName();
        settings->CurePoisonName = dataHandler->LookupForm(RE::FormID(0x109ADD), skyrimPluginName)->As<RE::EffectSetting>()->GetFullName();

        // Effect List
        RE::BGSListForm* Effect_List = dataHandler->LookupForm(RE::FormID(0x801), swiftPluginHame)->As<RE::BGSListForm>();

        // Populate a list of postive and negative effects
        for (auto& element : Effect_List->forms) {
            RE::EffectSetting* mEffect = element->As<RE::EffectSetting>();
            settings->AddEffect(mEffect->GetName(), mEffect->IsHostile());
        }
         
        // Effect Keywords
        utility->positiveKeyword = dataHandler->LookupForm(RE::FormID(0xF8A4E), skyrimPluginName)->As<RE::BGSKeyword>();
        utility->negativeKeyword = dataHandler->LookupForm(RE::FormID(0x42509), skyrimPluginName)->As<RE::BGSKeyword>();

        // Utility Forms
        RE::SpellItem* isWerewolfSpell = dataHandler->LookupForm(RE::FormID(0x805), swiftPluginHame)->As<RE::SpellItem>();
        utility->IsWerewolfConditions = &isWerewolfSpell->effects[0]->conditions;

        RE::SpellItem* isVampireSpell = dataHandler->LookupForm(RE::FormID(0x804), swiftPluginHame)->As<RE::SpellItem>();
        utility->IsVampireConditions = &isVampireSpell->effects[0]->conditions;

        // Quests
        utility->BrawlQuest = dataHandler->LookupForm(RE::FormID(0x47AE6), skyrimPluginName)->As<RE::TESQuest>();
    }

    void LoadCompatibilityForms(RE::TESDataHandler* dataHandler) {
        auto util = Utility::GetSingleton();

        if (dataHandler->LookupLoadedModByName(undeathPluginName)) {
            auto lichPerk = dataHandler->LookupForm(RE::FormID(0x3326D5), undeathPluginName);
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