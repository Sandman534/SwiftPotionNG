#include "Papyrus.h"
#include "Settings.h"

namespace {
    constexpr std::string_view PapyrusClass = "SwiftPotion";

    std::string stringToUpper(std::string oString){
       for(int i = 0; i < oString.length(); i++){
           oString[i] = toupper(oString[i]);
        }
        return oString;
    }

    PotionData& GetPotionData(std::string sSystem, int iHotkey) {
        if (iHotkey >= 0) {
            return Settings::GetSingleton()->HotkeyRecords[iHotkey];
        } else {
            // Health
            if (sSystem == "H") {
                return Settings::GetSingleton()->Health_Restore;
            } else if (sSystem == "HR") {
                return Settings::GetSingleton()->Health_Regen;
            } else if (sSystem == "HF") {
                return Settings::GetSingleton()->Health_Fortify;

            // Magicka
            } else if (sSystem == "M") {
                return Settings::GetSingleton()->Magicka_Restore;
            } else if (sSystem == "MR") {
                return Settings::GetSingleton()->Magicka_Regen;
            } else if (sSystem == "MF") {
                return Settings::GetSingleton()->Magicka_Fortify;

            // Stamina
            } else if (sSystem == "S") {
                return Settings::GetSingleton()->Stamina_Restore;
            } else if (sSystem == "SR") {
                return Settings::GetSingleton()->Stamina_Regen;
            } else if (sSystem == "SF") {
                return Settings::GetSingleton()->Stamina_Fortify;

            // Other Auto Effects
            } else if (sSystem == "CD") {
                return Settings::GetSingleton()->Cure_Disease;
            } else if (sSystem == "CP") {
                return Settings::GetSingleton()->Cure_Poison;

            // Default to Restore Health
            } else {
                return Settings::GetSingleton()->Health_Restore;
            }
        }
    }

    int Get_PotionDataInt(RE::StaticFunctionTag*, std::string sSystem, int iHotkey, std::string sParameter) {
        PotionData &WorkingData = GetPotionData(sSystem, iHotkey);
        std::string uParameter = stringToUpper(sParameter);

        if (uParameter == "HK") {
            return WorkingData.Hotkey;
        } else if (uParameter == "BV") {
            return WorkingData.BestValue;
        } else {
            return 0;
        }

    }

    void Set_PotionDataInt(RE::StaticFunctionTag*, std::string sSystem, int iHotkey, std::string sParameter, int aValue) {
        PotionData &WorkingData = GetPotionData(sSystem, iHotkey);
        std::string uParameter = stringToUpper(sParameter);

        if (uParameter == "HK") {
            WorkingData.Hotkey = aValue;
        } else if (uParameter == "BV") {
            WorkingData.BestValue = aValue;
        }
    }

    std::string Get_PotionDataString(RE::StaticFunctionTag*, std::string sSystem, int iHotkey, std::string sParameter) {
        PotionData &WorkingData = GetPotionData(sSystem, iHotkey);
        std::string uParameter = stringToUpper(sParameter);

        if (uParameter == "EF") {
            return WorkingData.EffectName;
        } else {
            return "";
        }

    }

    void Set_PotionDataString(RE::StaticFunctionTag*, std::string sSystem, int iHotkey, std::string sParameter, std::string aValue) {
        PotionData &WorkingData = GetPotionData(sSystem, iHotkey);
        std::string uParameter = stringToUpper(sParameter);

        if (uParameter == "EF") {
            WorkingData.EffectName = aValue;
        }
    }

    float Get_PotionDataFloat(RE::StaticFunctionTag*, std::string sSystem, int iHotkey, std::string sParameter) {
        PotionData &WorkingData = GetPotionData(sSystem, iHotkey);
        std::string uParameter = stringToUpper(sParameter);

        if (uParameter == "TH") {
            return WorkingData.Threshold;
        } else {
            return 0.0;
        }

    }

    void Set_PotionDataFloat(RE::StaticFunctionTag*, std::string sSystem, int iHotkey, std::string sParameter, float aValue) {
        PotionData &WorkingData = GetPotionData(sSystem, iHotkey);
        std::string uParameter = stringToUpper(sParameter);

        if (uParameter == "TH") {
            WorkingData.Threshold = aValue;
        }
    }

    bool Get_PotionDataBool(RE::StaticFunctionTag*, std::string sSystem, int iHotkey, std::string sParameter) {
        PotionData &WorkingData = GetPotionData(sSystem, iHotkey);
        std::string uParameter = stringToUpper(sParameter);

        if (uParameter == "UF") {
            return WorkingData.UseFood;
        } else if (uParameter == "PO") {
            return WorkingData.Poison;
        } else if (uParameter == "EB") {
            return WorkingData.Enabled;
        } else if (uParameter == "CO") {
            return WorkingData.CombatOnly;
        } else if (uParameter == "M1") {
            return WorkingData.Modifier1;
        } else if (uParameter == "M2") {
            return WorkingData.Modifier2;
        } else if (uParameter == "M3") {
            return WorkingData.Modifier3;
        } else {
            return false;
        }
    }

    void Set_PotionDataBool(RE::StaticFunctionTag*, std::string sSystem, int iHotkey, std::string sParameter, bool aValue) {
        PotionData &WorkingData = GetPotionData(sSystem, iHotkey);
        std::string uParameter = stringToUpper(sParameter);

        if (uParameter == "UF") {
            WorkingData.UseFood = aValue;
        } else if (uParameter == "PO") {
            WorkingData.Poison = aValue;
        } else if (uParameter == "EB") {
            WorkingData.Enabled = aValue;
        } else if (uParameter == "CO") {
            WorkingData.CombatOnly = aValue;
        } else if (uParameter == "M1") {
            WorkingData.Modifier1 = aValue;
        } else if (uParameter == "M2") {
            WorkingData.Modifier2 = aValue;
        } else if (uParameter == "M3") {
            WorkingData.Modifier3 = aValue;
        }
    }

    int HotkeyCount(RE::StaticFunctionTag*) { return Settings::GetSingleton()->HotkeyRecords.size(); }

    bool Get_SystemBool(RE::StaticFunctionTag*, std::string sParameter) {
        std::string uParameter = stringToUpper(sParameter);

        if (uParameter == "EB") {
            return Settings::GetSingleton()->SPNG_Enabled;
        } else if (uParameter == "NO") {
            return Settings::GetSingleton()->SPNG_Notifications;
        } else {
            return false;
        }
    }

    void Set_SystemBool(RE::StaticFunctionTag*, std::string sParameter, bool aValue) {
        std::string uParameter = stringToUpper(sParameter);

        if (uParameter == "EB") {
            Settings::GetSingleton()->SPNG_Enabled = aValue;
        } else if (uParameter == "NO") {
            Settings::GetSingleton()->SPNG_Notifications = aValue;
        } else if (uParameter == "HF") {
            Settings::GetSingleton()->SPNG_HotkeyFile = aValue;
        }
    }

    int Get_SystemInt(RE::StaticFunctionTag*, std::string sParameter) {
        std::string uParameter = stringToUpper(sParameter);

        if (uParameter == "M1") {
            return Settings::GetSingleton()->SPNG_Modifier1;
        } else if (uParameter == "M1") {
            return Settings::GetSingleton()->SPNG_Modifier2;
        } else if (uParameter == "M3") {
            return Settings::GetSingleton()->SPNG_Modifier3;
        } else {
            return 0;
        }
    }

    void Set_SystemInt(RE::StaticFunctionTag*, std::string sParameter, int aValue) {
        std::string uParameter = stringToUpper(sParameter);

        if (uParameter == "M1") {
            Settings::GetSingleton()->SPNG_Modifier1 = aValue;
        } else if (uParameter == "M1") {
            Settings::GetSingleton()->SPNG_Modifier2 = aValue;
        } else if (uParameter == "M3") {
            Settings::GetSingleton()->SPNG_Modifier3 = aValue;
        }
    }

    void SaveSettings(RE::StaticFunctionTag*) { 
        auto settings = Settings::GetSingleton();
        settings->SaveINI();
        if (settings->SPNG_HotkeyFile) {
			settings->SaveHotkeyINI();
		}
    }

    std::vector<std::string> GetEffectList(RE::StaticFunctionTag*, bool isNegative) {
        if (isNegative) {
            return Settings::GetSingleton()->Negative_Effects; 
        } else {
            return Settings::GetSingleton()->Positive_Effects; 
        }
    }
}

bool SwiftPotionExtension::RegisterSwiftPotion(RE::BSScript::IVirtualMachine* vm) {
    
    // Potion Data Functions
    vm->RegisterFunction("Get_PotionDataInt", PapyrusClass, Get_PotionDataInt);
    vm->RegisterFunction("Set_PotionDataInt", PapyrusClass, Set_PotionDataInt);
    vm->RegisterFunction("Get_PotionDataString", PapyrusClass, Get_PotionDataString);
    vm->RegisterFunction("Set_PotionDataString", PapyrusClass, Set_PotionDataString);
    vm->RegisterFunction("Get_PotionDataFloat", PapyrusClass, Get_PotionDataFloat);
    vm->RegisterFunction("Set_PotionDataFloat", PapyrusClass, Set_PotionDataFloat);
    vm->RegisterFunction("Get_PotionDataBool", PapyrusClass, Get_PotionDataBool);
    vm->RegisterFunction("Set_PotionDataBool", PapyrusClass, Set_PotionDataBool);

    // System Functions
    vm->RegisterFunction("Get_SystemBool", PapyrusClass, Get_SystemBool);
    vm->RegisterFunction("Set_SystemBool", PapyrusClass, Set_SystemBool);
    vm->RegisterFunction("Get_SystemInt", PapyrusClass, Get_SystemInt);
    vm->RegisterFunction("Set_SystemInt", PapyrusClass, Set_SystemInt);

    // General Functionality
    vm->RegisterFunction("HotkeyCount", PapyrusClass, HotkeyCount);
    vm->RegisterFunction("SaveSettings", PapyrusClass, SaveSettings);
    vm->RegisterFunction("GetEffectList", PapyrusClass, GetEffectList);

    return true;
}