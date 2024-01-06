#pragma once
#include "Settings.h"

class Utility {
public:
    RE::BGSListForm* SPNG_EffectList;

    RE::TESCondition* IsVampireConditions;
    RE::TESCondition* IsWerewolfConditions;

    RE::TESQuest* BrawlQuest;

    RE::BGSPerk* Undeath_LichPerk;

    RE::BGSKeyword* positiveKeyword;
    RE::BGSKeyword* negativeKeyword;

    uintptr_t PlayerSingletonAddress;
    uintptr_t UISingletonAddress;
    uintptr_t MenuControlsSingletonAddress;

    static Utility* GetSingleton() {
        static Utility playerStatus;
        return &playerStatus;
    }

    static RE::PlayerCharacter* GetPlayer() {
        REL::Relocation<RE::NiPointer<RE::PlayerCharacter>*> singleton{Utility::GetSingleton()->PlayerSingletonAddress};
        return singleton->get();
    }

    static RE::UI* GetUI() {
        REL::Relocation<RE::NiPointer<RE::UI>*> singleton{Utility::GetSingleton()->UISingletonAddress};
        return singleton->get();
    }

    static RE::MenuControls* GetMenuControls() {
        REL::Relocation<RE::NiPointer<RE::MenuControls>*> singleton{
            Utility::GetSingleton()->MenuControlsSingletonAddress};
        return singleton->get();
    }

    static void ShowNotification(std::string msg, bool messageBox = false) {
        if (messageBox) {
            RE::DebugMessageBox(msg.c_str());
        } else {
            RE::DebugNotification(msg.c_str());
        }
    }

    static void StopperCheck(RE::AlchemyItem* pPotion) {
        const auto settings = Settings::GetSingleton();

        for (auto& eEffect : pPotion->effects) {
            //settings->AddEffect(eEffect->baseEffect->GetFullName(), eEffect->IsHostile());

		    // Health
		    if (stricmp(eEffect->baseEffect->GetFullName(),settings->Health_Restore.EffectName.c_str()) == 0 && settings->Health_Restore.Stopper) {
			    settings->Health_Restore.Stopper = false;
            } else if (stricmp(eEffect->baseEffect->GetFullName(),settings->Health_Regen.EffectName.c_str()) == 0 && settings->Health_Regen.Stopper) {
			    settings->Health_Regen.Stopper = false;
		    } else if (stricmp(eEffect->baseEffect->GetFullName(),settings->Health_Fortify.EffectName.c_str()) == 0 && settings->Health_Fortify.Stopper) {
			    settings->Health_Fortify.Stopper = false;

		    // Magicka
		    } else if (stricmp(eEffect->baseEffect->GetFullName(),settings->Magicka_Restore.EffectName.c_str()) == 0 && settings->Magicka_Restore.Stopper){
			    settings->Magicka_Restore.Stopper = false;
		    } else if (stricmp(eEffect->baseEffect->GetFullName(),settings->Magicka_Regen.EffectName.c_str()) == 0 && settings->Magicka_Regen.Stopper) {
			    settings->Magicka_Regen.Stopper = false;
		    } else if (stricmp(eEffect->baseEffect->GetFullName(),settings->Magicka_Fortify.EffectName.c_str()) == 0 && settings->Magicka_Fortify.Stopper) {
			    settings->Magicka_Fortify.Stopper = false;

		    // Stamina
		    } else if (stricmp(eEffect->baseEffect->GetFullName(),settings->Stamina_Restore.EffectName.c_str()) == 0 && settings->Stamina_Restore.Stopper) {
			    settings->Stamina_Restore.Stopper = false;
		    } else if (stricmp(eEffect->baseEffect->GetFullName(),settings->Stamina_Regen.EffectName.c_str()) == 0 && settings->Stamina_Regen.Stopper) {
			    settings->Stamina_Regen.Stopper = false;
		    } else if (stricmp(eEffect->baseEffect->GetFullName(),settings->Stamina_Fortify.EffectName.c_str()) == 0 && settings->Stamina_Fortify.Stopper) {
			    settings->Stamina_Fortify.Stopper = false;

            // Other Effects
		    } else if (stricmp(eEffect->baseEffect->GetFullName(),settings->Cure_Disease.EffectName.c_str()) == 0 && settings->Cure_Disease.Stopper) {
			    settings->Stamina_Restore.Stopper = false;
		    } else if (stricmp(eEffect->baseEffect->GetFullName(),settings->Cure_Poison.EffectName.c_str()) == 0 && settings->Cure_Poison.Stopper) {
			    settings->Stamina_Regen.Stopper = false;
            }
        };
    }

    // Player checks
    static bool PlayerIsWerewolf() {
        auto util = Utility::GetSingleton();
        return util->IsWerewolfConditions->IsTrue(util->GetPlayer(), nullptr);
    }

    static bool PlayerIsVampire() {
        auto util = Utility::GetSingleton();
        return util->IsVampireConditions->IsTrue(GetPlayer(), nullptr);
    }

    static bool PlayerIsLich() {
        auto util = Utility::GetSingleton();
        if (util->Undeath_LichPerk) {
            return GetPlayer()->HasPerk(util->Undeath_LichPerk);
        } else {
            return false;
        }
    }

    static bool IsPlayerInBrawl() {
        auto util = Utility::GetSingleton();

        if (util->BrawlQuest->currentStage <= 0 || util->BrawlQuest->currentStage >= 250) {
            return false;
        } else {
            return true;
        }
    }

    static bool IsPlayerInDialogue() {
        return Utility::GetSingleton()->GetUI()->IsMenuOpen(RE::DialogueMenu::MENU_NAME);
    }

    // Attribute Functions
    static bool PlayerIsAlive() {
        return (Utility::GetSingleton()->GetPlayer()->AsActorValueOwner()->GetActorValue(RE::ActorValue::kHealth) > 0);
    }

    static float GetPlayerAttribute(int iAttribute) {
        RE::ActorValue workingValue;
        if (iAttribute == 0) {
            workingValue = RE::ActorValue::kHealth;
        } else if (iAttribute == 1) {
            workingValue = RE::ActorValue::kMagicka;
        } else {
            workingValue = RE::ActorValue::kStamina;
        }

        return GetPlayer()->AsActorValueOwner()->GetActorValue(workingValue) / (GetPlayer()->AsActorValueOwner()->GetPermanentActorValue(workingValue) +
            GetPlayer()->As<RE::Actor>()->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, workingValue));
    }

    static float GetPlayerDifference(int iAttribute) {
        RE::ActorValue workingValue;
        if (iAttribute == 0) {
            workingValue = RE::ActorValue::kHealth;
        } else if (iAttribute == 1) {
            workingValue = RE::ActorValue::kMagicka;
        } else {
            workingValue = RE::ActorValue::kStamina;
        }

        return (GetPlayer()->AsActorValueOwner()->GetPermanentActorValue(workingValue) +
            GetPlayer()->As<RE::Actor>()->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, workingValue)) - GetPlayer()->AsActorValueOwner()->GetActorValue(workingValue);
    }
};
