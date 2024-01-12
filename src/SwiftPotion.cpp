#include "Hooks.h"
#include "SwiftPotion.h"
#include "Utility.h"
#include "Settings.h"

std::int32_t SwiftPotion::OnUpdate() {
    if (!Utility::GetUI()->GameIsPaused()) {
        if (g_deltaTime > 0) {
            lastTime += g_deltaTime;
            if (lastTime >= 1.0f) {
                SwiftPotionLoopUpdate();
                lastTime = 0;
            }
        }
    }

    return _OnUpdate();
}

bool SwiftPotion::InstallUpdateHook() {
    auto& trampoline = SKSE::GetTrampoline();
    _OnUpdate = trampoline.write_call<5>(Hooks::On_Update_Hook.address(), OnUpdate);
    logger::info("Installed update hook");
    return true;
}

void SwiftPotion::SwiftPotionLoopUpdate() {
    auto utility = Utility::GetSingleton();
    auto settings = Settings::GetSingleton();

    if (settings->SPNG_Enabled && !utility->PlayerIsWerewolf() && utility->PlayerIsAlive() && !utility->IsPlayerInBrawl()) 
    {
        // Auto Health System
        AutoSystemCheck(settings->Health_Restore);
        AutoSystemCheck(settings->Health_Regen);
        AutoSystemCheck(settings->Health_Fortify);

        // Auto Magicka System
        AutoSystemCheck(settings->Magicka_Restore);
        AutoSystemCheck(settings->Magicka_Regen);
        AutoSystemCheck(settings->Magicka_Fortify);

        // Auto Stamina System
        AutoSystemCheck(settings->Stamina_Restore);
        AutoSystemCheck(settings->Stamina_Regen);
        AutoSystemCheck(settings->Stamina_Fortify);
    }

    if (settings->SPNG_Enabled && utility->PlayerIsAlive() && !utility->PlayerIsWerewolf() && !utility->PlayerIsVampire()) {
        if (settings->Cure_Disease.Enabled && !settings->Cure_Disease.Stopper && CheckSpecificEffects(0)) {
            UsePotion(utility->GetPlayer(), settings->Cure_Disease, false);
        }
        if (settings->Cure_Poison.Enabled && !settings->Cure_Poison.Stopper && CheckSpecificEffects(1)) {
            UsePotion(utility->GetPlayer(), settings->Cure_Poison, false);
        }
    }
}

void SwiftPotion::AutoSystemCheck(PotionData &SystemData) {
    auto utility = Utility::GetSingleton();

    // Run check for potion use
    if (SystemData.Enabled && !SystemData.Stopper) {
        if (utility->GetPlayerAttribute(SystemData.Attribute) <= SystemData.Threshold) {
            if ((SystemData.CombatOnly && utility->GetPlayer()->IsInCombat()) || !SystemData.CombatOnly) {
                if (!AutoSystemEffectCheck(SystemData.EffectName)) {
                    UsePotion(utility->GetPlayer(), SystemData, false);
                }
            }
        }
    }
}

bool SwiftPotion::AutoSystemEffectCheck(std::string sEffect) {
    auto utility = Utility::GetSingleton();
    RE::Actor* aPlayer = Utility::GetPlayer();

    // Check to see if the player has the effect listed
    for (auto& eEffect : *aPlayer->AsMagicTarget()->GetActiveEffectList()) {
        if (strcmpi(eEffect->GetBaseObject()->GetFullName(),sEffect.c_str()) == 0 && eEffect->GetBaseObject()->HasKeyword(utility->positiveKeyword)) {
            return true;
        }
    }

    return false;
}

bool SwiftPotion::CheckSpecificEffects(int iCheckType) {
    auto utility = Utility::GetSingleton();
    RE::Actor* aPlayer = Utility::GetPlayer();

    // Check to see if the player has the effect listed
    for (auto& eEffect : *aPlayer->AsMagicTarget()->GetActiveEffectList()) {

        if (iCheckType == 0 && eEffect->spell->GetSpellType() == RE::MagicSystem::SpellType::kDisease) {
            return true;
        } else if (iCheckType == 1 && eEffect->GetBaseObject()->data.resistVariable == RE::ActorValue::kPoisonResist) {
            return true;
        }
    }

    return false;
}

void SwiftPotion::ProcessHotkey(const uint32_t& _code, bool _modifier1, bool _modifier2, bool _modifier3) {
    auto utility = Utility::GetSingleton();
    auto settings = Settings::GetSingleton();

    if (settings->Health_Restore.Hotkey == _code && settings->Health_Restore.Modifier1 == _modifier1 && settings->Health_Restore.Modifier2 == _modifier2 && settings->Health_Restore.Modifier3 == _modifier3) {
        UsePotionAutoHotkey(utility->GetPlayer(), settings->Health_Restore, settings->Health_Regen, settings->Health_Fortify, "Health");
        //UsePotion(utility->GetPlayer(), settings->Health_Restore, true);
    } else if (settings->Magicka_Restore.Hotkey == _code && settings->Magicka_Restore.Modifier1 == _modifier1 && settings->Magicka_Restore.Modifier2 == _modifier2 && settings->Magicka_Restore.Modifier3 == _modifier3) {
        UsePotionAutoHotkey(utility->GetPlayer(), settings->Magicka_Restore, settings->Magicka_Regen, settings->Magicka_Fortify, "Magicka");
        //UsePotion(utility->GetPlayer(), settings->Magicka_Restore, true);
    } else if (settings->Stamina_Restore.Hotkey == _code && settings->Stamina_Restore.Modifier1 == _modifier1 && settings->Stamina_Restore.Modifier2 == _modifier2 && settings->Stamina_Restore.Modifier3 == _modifier3) {
        UsePotionAutoHotkey(utility->GetPlayer(), settings->Stamina_Restore, settings->Stamina_Regen, settings->Stamina_Fortify, "Stamina");
        //UsePotion(utility->GetPlayer(), settings->Stamina_Restore, true);
    } else {
        for (auto hotkeyData : settings->HotkeyRecords) {
            if (hotkeyData.Hotkey == _code && hotkeyData.Modifier1 == _modifier1 && hotkeyData.Modifier2 == _modifier2 && hotkeyData.Modifier3 == _modifier3) {
                UsePotion(utility->GetPlayer(), hotkeyData, true);
            }
        }
    }
}

void SwiftPotion::UsePotion(RE::Actor* aPlayer, PotionData &SystemData, bool bHotkey) {
	// Setup constants
    auto utility = Utility::GetSingleton();
    auto settings = Settings::GetSingleton();

    std::string sEffect = SystemData.EffectName;

    // Get the correct effect list based on type
	RE::AlchemyItem* pPotion = GetPotion(aPlayer, SystemData);

	// Use the potion, or stop the Auto system from running until another enteres the user inventory
	if (pPotion) {
        RE::ActorEquipManager::GetSingleton()->EquipObject(aPlayer, pPotion, nullptr, 1, nullptr, true);

	// Disable Monitoring system till new potion enters players inventory
    } else if (!pPotion && !bHotkey) {
        SystemData.Stopper = true;
    }

    // Display notification if turned on
    if (!pPotion && settings->SPNG_Notifications && !SystemData.Poison) {
    	utility->ShowNotification("You have no " + SystemData.EffectName + " potions");
    } else if (!pPotion && settings->SPNG_Notifications && SystemData.Poison) {
    	utility->ShowNotification("You have no " + SystemData.EffectName + " poisons");
    }
}

void SwiftPotion::UsePotionAutoHotkey(RE::Actor* aPlayer, PotionData &RestoreData, PotionData &RegenData, PotionData &FortifyData, std::string EffectName) {
	// Setup constants
    auto utility = Utility::GetSingleton();
    auto settings = Settings::GetSingleton();

    // Get the correct effect list based on type
	RE::AlchemyItem* restorePotion = GetPotion(aPlayer, RestoreData);
    RE::AlchemyItem* regenPotion = GetPotion(aPlayer, RegenData);
    RE::AlchemyItem* fortifyPotion = GetPotion(aPlayer, FortifyData);

    // Display notification if turned on
    if (!restorePotion && !regenPotion && !fortifyPotion && settings->SPNG_Notifications) {
    	utility->ShowNotification("You have no " + EffectName + " effect potions");
        return;
    }

    if (FortifyData.Enabled && utility->GetPlayerAttribute(FortifyData.Attribute) <= FortifyData.Threshold && fortifyPotion) {
        RE::ActorEquipManager::GetSingleton()->EquipObject(aPlayer, fortifyPotion, nullptr, 1, nullptr, true);
    } else if (RestoreData.Enabled && utility->GetPlayerAttribute(RestoreData.Attribute) <= RestoreData.Threshold && restorePotion) {
        RE::ActorEquipManager::GetSingleton()->EquipObject(aPlayer, restorePotion, nullptr, 1, nullptr, true);
    } else if (RegenData.Enabled && utility->GetPlayerAttribute(RegenData.Attribute) <= RegenData.Threshold && regenPotion) {
        RE::ActorEquipManager::GetSingleton()->EquipObject(aPlayer, regenPotion, nullptr, 1, nullptr, true);
    } else if (RestoreData.Enabled && restorePotion) {
        RE::ActorEquipManager::GetSingleton()->EquipObject(aPlayer, restorePotion, nullptr, 1, nullptr, true);
    } else if (RegenData.Enabled && regenPotion) {
        RE::ActorEquipManager::GetSingleton()->EquipObject(aPlayer, regenPotion, nullptr, 1, nullptr, true);
    } else if (FortifyData.Enabled && fortifyPotion) {
        RE::ActorEquipManager::GetSingleton()->EquipObject(aPlayer, fortifyPotion, nullptr, 1, nullptr, true);
    }

}

RE::AlchemyItem* SwiftPotion::GetPotion(RE::Actor* aPlayer, PotionData &SystemData) {
    auto utility = Utility::GetSingleton();

    // Set up local variables
    RE::AlchemyItem* pFinalPotion = nullptr;

    // Magnitude needs to be set based on High/Low Option
    float iMagnitude = NULL;

    // Get reference Inventory
    auto inv = aPlayer->GetInventory();
    for (const auto& [item, data] : inv) {
        if (!item->Is(RE::FormType::AlchemyItem)) {
            continue;
        }

        const auto& [count, entry] = data;
        if (count > 0 && entry) {
            // Found Potion
            RE::AlchemyItem* pPotion = item->As<RE::AlchemyItem>();

            // If the potion doesnt meet our requirements
            if ((pPotion->IsFood() && !SystemData.UseFood) || (pPotion->IsPoison() && !SystemData.Poison)) {
                continue;
            }

            // Loop through all of the magic effects on the potion
            for (auto& eEffect : pPotion->effects) {
                if (eEffect->baseEffect->GetFullName() == SystemData.EffectName) {
                    if (!iMagnitude || (SystemData.BestValue == 0 && eEffect->effectItem.magnitude > iMagnitude) || (SystemData.BestValue == 1 && eEffect->effectItem.magnitude < iMagnitude)) {
                        iMagnitude = eEffect->effectItem.magnitude;
                        pFinalPotion = pPotion;
                    } else if (SystemData.BestValue == 2) {
                        // If the potion has no duration, set it to 1
                        float potionDuration = NULL;
                        if (eEffect->effectItem.duration <= 0) {
                            potionDuration = 1.0;
                        } else {
                            potionDuration = eEffect->effectItem.duration;
                        }
                        float healthGap = abs(utility->GetPlayerDifference(SystemData.Attribute) - (eEffect->effectItem.magnitude * potionDuration));
                        if (healthGap < iMagnitude) {
                            iMagnitude = eEffect->effectItem.magnitude;
                            pFinalPotion = pPotion;
                        }
                    }
                }
            }
        }
    }

    return pFinalPotion;
}