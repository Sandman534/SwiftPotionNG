#pragma once
#include "SimpleIni.h"
#include <format>

struct PotionData {
	std::string EffectName = "";
    bool UseFood = false;
	bool Poison = false;
	int BestValue = 0;

	// Auto System Values
	bool Enabled = false;
	bool CombatOnly = false;
	float Threshold = 0;
	int Attribute = 0;
	bool Stopper = false;

	// Hotkey Values
	int Hotkey = 0;
	bool Modifier1 = false;
	bool Modifier2 = false;
	bool Modifier3 = false;

	bool solve(std::string s) {
		return s != "false";
	}

	std::string Serialize() {
		std::string returnValue = std::format("{},", EffectName);
		returnValue += std::format("{},", UseFood);
		returnValue += std::format("{},", Poison);
		returnValue += std::format("{},", BestValue);
		returnValue += std::format("{},", Hotkey);
		returnValue += std::format("{},", Modifier1);
		returnValue += std::format("{},", Modifier2);
		returnValue += std::format("{},", Modifier3);

		return returnValue;
	}

	void Deserialize(std::string sData) {
		size_t pos = 0;
		std::string token;
		std::string delimiter = ",";
		int idx = 0;

		while ((pos = sData.find(delimiter)) != std::string::npos) {
			token = sData.substr(0, pos);
			switch (idx) {
				case 0:
					EffectName = token;
					break;
				case 1:
					UseFood = solve(token);
					break;
				case 2:
					Poison = solve(token);
					break;
				case 3:
					BestValue = std::stoi(token);
					break;
				case 4:
					Hotkey = std::stoi(token);
					break;
				case 5:
					Modifier1 = solve(token);
					break;
				case 6:
					Modifier2 = solve(token);
					break;
				case 7:
					Modifier3 = solve(token);
					break;
			}

			sData.erase(0, pos + delimiter.length());
			idx++;
		}
	}
};

class Settings
{
public:
	[[nodiscard]] static Settings* GetSingleton()
	{
		static Settings singleton;
		return std::addressof(singleton);
	}

	void LoadSettings()
	{
		// Default Hotkey Assignments
		LoadHotkeys();

		// Reset Auto System names based on Skyrim.esp
		DefaultAutoData(Health_Restore,"H",HealthRestoreName);
		DefaultAutoData(Health_Regen,"HR",HealthRegenName);
		DefaultAutoData(Health_Fortify,"HF",HealthFortifyName);

		DefaultAutoData(Magicka_Restore,"M",MagickaRestoreName);
		DefaultAutoData(Magicka_Regen,"MR",MagickaRegenName);
		DefaultAutoData(Magicka_Fortify,"MF",MagickaFortifyName);

		DefaultAutoData(Stamina_Restore,"S",StaminaRestoreName);
		DefaultAutoData(Stamina_Regen,"SR",StaminaRegenName);
		DefaultAutoData(Stamina_Fortify,"SF",StaminaFortifyName);

		// Set additional effects
		Cure_Disease.EffectName = CureDiseaseName;
		Cure_Disease.Enabled = false;
		Cure_Poison.EffectName = CurePoisonName;
		Cure_Poison.Enabled = false;

		// Load INI Settings
		LoadINI();

		// Load Hotkeys
		if (SPNG_HotkeyFile) {
			LoadHotkeyINI();
		}
	}

	void LoadINI()
	{	
		constexpr auto settings_path = L"Data/SKSE/Plugins/SwiftPotionNG.ini";

		CSimpleIniA iniSettings;
		iniSettings.SetUnicode();
		iniSettings.LoadFile(settings_path);

		detail::get_value(iniSettings, SPNG_Enabled, "General", "SystemEnabled");
		detail::get_value(iniSettings, SPNG_Notifications, "General", "NotificationsEnabled");
		detail::get_value(iniSettings, SPNG_HotkeyFile, "General", "HotkeyPersistence");

		// Health Auto System
		detail::get_value(iniSettings, Health_Restore.EffectName, "Health System", "H_EffectName");
		detail::get_value(iniSettings, Health_Restore.UseFood, "Health System", "H_FoodUse");
		detail::get_value(iniSettings, Health_Restore.BestValue, "Health System", "H_BestValue");
		detail::get_value(iniSettings, Health_Restore.Enabled, "Health System", "H_Enabled");
		detail::get_value(iniSettings, Health_Restore.CombatOnly, "Health System", "H_CombatOnly");
		detail::get_value(iniSettings, Health_Restore.Threshold, "Health System", "H_Threshold");
		detail::get_value(iniSettings, Health_Restore.Hotkey, "Health System", "H_Hotkey");
		detail::get_value(iniSettings, Health_Restore.Modifier1, "Health System", "H_Modifier1");
		detail::get_value(iniSettings, Health_Restore.Modifier2, "Health System", "H_Modifier2");
		detail::get_value(iniSettings, Health_Restore.Modifier3, "Health System", "H_Modifier3");

		// Regeneration
		detail::get_value(iniSettings, Health_Regen.EffectName, "Health System", "HR_EffectName");
		detail::get_value(iniSettings, Health_Regen.UseFood, "Health System", "HR_FoodUse");
		detail::get_value(iniSettings, Health_Regen.BestValue, "Health System", "HR_BestValue");
		detail::get_value(iniSettings, Health_Regen.Enabled, "Health System", "HR_Enabled");
		detail::get_value(iniSettings, Health_Regen.CombatOnly, "Health System", "HR_CombatOnly");
		detail::get_value(iniSettings, Health_Regen.Threshold, "Health System", "HR_Threshold");

		// Fortification
		detail::get_value(iniSettings, Health_Fortify.EffectName, "Health System", "HF_EffectName");
		detail::get_value(iniSettings, Health_Fortify.UseFood, "Health System", "HF_FoodUse");
		detail::get_value(iniSettings, Health_Fortify.BestValue, "Health System", "HF_BestValue");
		detail::get_value(iniSettings, Health_Fortify.Enabled, "Health System", "HF_Enabled");
		detail::get_value(iniSettings, Health_Fortify.CombatOnly, "Health System", "HF_CombatOnly");
		detail::get_value(iniSettings, Health_Fortify.Threshold, "Health System", "HF_Threshold");

		// Magicka Auto System
		detail::get_value(iniSettings, Magicka_Restore.EffectName, "Magicka System", "M_EffectName");
		detail::get_value(iniSettings, Magicka_Restore.UseFood, "Magicka System", "M_FoodUse");
		detail::get_value(iniSettings, Magicka_Restore.BestValue, "Magicka System", "M_BestValue");
		detail::get_value(iniSettings, Magicka_Restore.Enabled, "Magicka System", "M_Enabled");
		detail::get_value(iniSettings, Magicka_Restore.CombatOnly, "Magicka System", "M_CombatOnly");
		detail::get_value(iniSettings, Magicka_Restore.Threshold, "Magicka System", "M_Threshold");
		detail::get_value(iniSettings, Magicka_Restore.Hotkey, "Magicka System", "M_Hotkey");
		detail::get_value(iniSettings, Magicka_Restore.Modifier1, "Magicka System", "M_Modifier1");
		detail::get_value(iniSettings, Magicka_Restore.Modifier2, "Magicka System", "M_Modifier2");
		detail::get_value(iniSettings, Magicka_Restore.Modifier3, "Magicka System", "M_Modifier3");

		// Regeneration
		detail::get_value(iniSettings, Magicka_Regen.EffectName, "Magicka System", "MR_EffectName");
		detail::get_value(iniSettings, Magicka_Regen.UseFood, "Magicka System", "MR_FoodUse");
		detail::get_value(iniSettings, Magicka_Regen.BestValue, "Magicka System", "MR_BestValue");
		detail::get_value(iniSettings, Magicka_Regen.Enabled, "Magicka System", "MR_Enabled");
		detail::get_value(iniSettings, Magicka_Regen.CombatOnly, "Magicka System", "MR_CombatOnly");
		detail::get_value(iniSettings, Magicka_Regen.Threshold, "Magicka System", "MR_Threshold");

		// Fortification
		detail::get_value(iniSettings, Magicka_Fortify.EffectName, "Magicka System", "MF_EffectName");
		detail::get_value(iniSettings, Magicka_Fortify.UseFood, "Magicka System", "MF_FoodUse");
		detail::get_value(iniSettings, Magicka_Fortify.BestValue, "Magicka System", "MF_BestValue");
		detail::get_value(iniSettings, Magicka_Fortify.Enabled, "Magicka System", "MF_Enabled");
		detail::get_value(iniSettings, Magicka_Fortify.CombatOnly, "Magicka System", "MF_CombatOnly");
		detail::get_value(iniSettings, Magicka_Fortify.Threshold, "Magicka System", "MF_Threshold");

		// Stamina Auto System
		detail::get_value(iniSettings, Stamina_Restore.EffectName, "Stamina System", "S_EffectName");
		detail::get_value(iniSettings, Stamina_Restore.UseFood, "Stamina System", "S_FoodUse");
		detail::get_value(iniSettings, Stamina_Restore.BestValue, "Stamina System", "S_BestValue");
		detail::get_value(iniSettings, Stamina_Restore.Enabled, "Stamina System", "S_Enabled");
		detail::get_value(iniSettings, Stamina_Restore.CombatOnly, "Stamina System", "S_CombatOnly");
		detail::get_value(iniSettings, Stamina_Restore.Threshold, "Stamina System", "S_Threshold");
		detail::get_value(iniSettings, Stamina_Restore.Hotkey, "Stamina System", "S_Hotkey");
		detail::get_value(iniSettings, Stamina_Restore.Modifier1, "Stamina System", "S_Modifier1");
		detail::get_value(iniSettings, Stamina_Restore.Modifier2, "Stamina System", "S_Modifier2");
		detail::get_value(iniSettings, Stamina_Restore.Modifier3, "Stamina System", "S_Modifier3");

		// Regeneration
		detail::get_value(iniSettings, Stamina_Regen.EffectName, "Stamina System", "SR_EffectName");
		detail::get_value(iniSettings, Stamina_Regen.UseFood, "Stamina System", "SR_FoodUse");
		detail::get_value(iniSettings, Stamina_Regen.BestValue, "Stamina System", "SR_BestValue");
		detail::get_value(iniSettings, Stamina_Regen.Enabled, "Stamina System", "SR_Enabled");
		detail::get_value(iniSettings, Stamina_Regen.CombatOnly, "Stamina System", "SR_CombatOnly");
		detail::get_value(iniSettings, Stamina_Regen.Threshold, "Stamina System", "SR_Threshold");

		// Fortification
		detail::get_value(iniSettings, Stamina_Fortify.EffectName, "Stamina System", "SF_EffectName");
		detail::get_value(iniSettings, Stamina_Fortify.UseFood, "Stamina System", "SF_FoodUse");
		detail::get_value(iniSettings, Stamina_Fortify.BestValue, "Stamina System", "SF_BestValue");
		detail::get_value(iniSettings, Stamina_Fortify.Enabled, "Stamina System", "SF_Enabled");
		detail::get_value(iniSettings, Stamina_Fortify.CombatOnly, "Stamina System", "SF_CombatOnly");
		detail::get_value(iniSettings, Stamina_Fortify.Threshold, "Stamina System", "SF_Threshold");

		// Additional Effects
		detail::get_value(iniSettings, Cure_Disease.EffectName, "Other Auto Effects", "Disease_EffectName");
		detail::get_value(iniSettings, Cure_Disease.Enabled, "Other Auto Effects", "Disease_Enabled");
		detail::get_value(iniSettings, Cure_Poison.EffectName, "Other Auto Effects", "Poison_EffectName");
		detail::get_value(iniSettings, Cure_Poison.Enabled, "Other Auto Effects", "Poison_Enabled");

		iniSettings.SaveFile(settings_path);
	}

	void SaveINI()
	{	
		constexpr auto settings_path = L"Data/SKSE/Plugins/SwiftPotionNG.ini";

		CSimpleIniA iniSettings;
		iniSettings.SetUnicode();
		iniSettings.LoadFile(settings_path);

		detail::set_value(iniSettings, SPNG_Enabled, "General", "SystemEnabled");
		detail::set_value(iniSettings, SPNG_Notifications, "General", "NotificationsEnabled");
		detail::set_value(iniSettings, SPNG_HotkeyFile, "General", "HotkeyPersistence");

		// Health Auto System
		detail::set_value(iniSettings, Health_Restore.EffectName, "Health System", "H_EffectName");
		detail::set_value(iniSettings, Health_Restore.UseFood, "Health System", "H_FoodUse");
		detail::set_value(iniSettings, Health_Restore.BestValue, "Health System", "H_BestValue");
		detail::set_value(iniSettings, Health_Restore.Enabled, "Health System", "H_Enabled");
		detail::set_value(iniSettings, Health_Restore.CombatOnly, "Health System", "H_CombatOnly");
		detail::set_value(iniSettings, Health_Restore.Threshold, "Health System", "H_Threshold");
		detail::set_value(iniSettings, Health_Restore.Hotkey, "Health System", "H_Hotkey");
		detail::set_value(iniSettings, Health_Restore.Modifier1, "Health System", "H_Modifier1");
		detail::set_value(iniSettings, Health_Restore.Modifier2, "Health System", "H_Modifier2");
		detail::set_value(iniSettings, Health_Restore.Modifier3, "Health System", "H_Modifier3");

		// Regeneration
		detail::set_value(iniSettings, Health_Regen.EffectName, "Health System", "HR_EffectName");
		detail::set_value(iniSettings, Health_Regen.UseFood, "Health System", "HR_FoodUse");
		detail::set_value(iniSettings, Health_Regen.Poison, "Health System", "HR_CombatOnly");
		detail::set_value(iniSettings, Health_Regen.BestValue, "Health System", "HR_BestValue");
		detail::set_value(iniSettings, Health_Regen.Enabled, "Health System", "HR_Enabled");
		detail::set_value(iniSettings, Health_Regen.CombatOnly, "Health System", "HR_CombatOnly");
		detail::set_value(iniSettings, Health_Regen.Threshold, "Health System", "HR_Threshold");

		// Fortification
		detail::set_value(iniSettings, Health_Fortify.EffectName, "Health System", "HF_EffectName");
		detail::set_value(iniSettings, Health_Fortify.UseFood, "Health System", "HF_FoodUse");
		detail::set_value(iniSettings, Health_Fortify.Poison, "Health System", "HF_CombatOnly");
		detail::set_value(iniSettings, Health_Fortify.BestValue, "Health System", "HF_BestValue");
		detail::set_value(iniSettings, Health_Fortify.Enabled, "Health System", "HF_Enabled");
		detail::set_value(iniSettings, Health_Fortify.CombatOnly, "Health System", "HF_CombatOnly");
		detail::set_value(iniSettings, Health_Fortify.Threshold, "Health System", "HF_Threshold");

		// Magicka Auto System
		detail::set_value(iniSettings, Magicka_Restore.EffectName, "Magicka System", "M_EffectName");
		detail::set_value(iniSettings, Magicka_Restore.UseFood, "Magicka System", "M_FoodUse");
		detail::set_value(iniSettings, Magicka_Restore.BestValue, "Magicka System", "M_BestValue");
		detail::set_value(iniSettings, Magicka_Restore.Enabled, "Magicka System", "M_Enabled");
		detail::set_value(iniSettings, Magicka_Restore.CombatOnly, "Magicka System", "M_CombatOnly");
		detail::set_value(iniSettings, Magicka_Restore.Threshold, "Magicka System", "M_Threshold");
		detail::set_value(iniSettings, Magicka_Restore.Hotkey, "Magicka System", "M_Hotkey");
		detail::set_value(iniSettings, Magicka_Restore.Modifier1, "Magicka System", "M_Modifier1");
		detail::set_value(iniSettings, Magicka_Restore.Modifier2, "Magicka System", "M_Modifier2");
		detail::set_value(iniSettings, Magicka_Restore.Modifier3, "Magicka System", "M_Modifier3");

		// Regeneration
		detail::set_value(iniSettings, Magicka_Regen.EffectName, "Magicka System", "MR_EffectName");
		detail::set_value(iniSettings, Magicka_Regen.UseFood, "Magicka System", "MR_FoodUse");
		detail::set_value(iniSettings, Magicka_Regen.Poison, "Magicka System", "MR_CombatOnly");
		detail::set_value(iniSettings, Magicka_Regen.BestValue, "Magicka System", "MR_BestValue");
		detail::set_value(iniSettings, Magicka_Regen.Enabled, "Magicka System", "MR_Enabled");
		detail::set_value(iniSettings, Magicka_Regen.CombatOnly, "Magicka System", "MR_CombatOnly");
		detail::set_value(iniSettings, Magicka_Regen.Threshold, "Magicka System", "MR_Threshold");

		// Fortification
		detail::set_value(iniSettings, Magicka_Fortify.EffectName, "Magicka System", "MF_EffectName");
		detail::set_value(iniSettings, Magicka_Fortify.UseFood, "Magicka System", "MF_FoodUse");
		detail::set_value(iniSettings, Magicka_Fortify.Poison, "Magicka System", "MF_CombatOnly");
		detail::set_value(iniSettings, Magicka_Fortify.BestValue, "Magicka System", "MF_BestValue");
		detail::set_value(iniSettings, Magicka_Fortify.Enabled, "Magicka System", "MF_Enabled");
		detail::set_value(iniSettings, Magicka_Fortify.CombatOnly, "Magicka System", "MF_CombatOnly");
		detail::set_value(iniSettings, Magicka_Fortify.Threshold, "Magicka System", "MF_Threshold");

		// Stamina Auto System
		detail::set_value(iniSettings, Stamina_Restore.EffectName, "Stamina System", "S_EffectName");
		detail::set_value(iniSettings, Stamina_Restore.UseFood, "Stamina System", "S_FoodUse");
		detail::set_value(iniSettings, Stamina_Restore.BestValue, "Stamina System", "S_BestValue");
		detail::set_value(iniSettings, Stamina_Restore.Enabled, "Stamina System", "S_Enabled");
		detail::set_value(iniSettings, Stamina_Restore.CombatOnly, "Stamina System", "S_CombatOnly");
		detail::set_value(iniSettings, Stamina_Restore.Threshold, "Stamina System", "S_Threshold");
		detail::set_value(iniSettings, Stamina_Restore.Hotkey, "Stamina System", "S_Hotkey");
		detail::set_value(iniSettings, Stamina_Restore.Modifier1, "Stamina System", "S_Modifier1");
		detail::set_value(iniSettings, Stamina_Restore.Modifier2, "Stamina System", "S_Modifier2");
		detail::set_value(iniSettings, Stamina_Restore.Modifier3, "Stamina System", "S_Modifier3");

		// Regeneration
		detail::set_value(iniSettings, Stamina_Regen.EffectName, "Stamina System", "SR_EffectName");
		detail::set_value(iniSettings, Stamina_Regen.UseFood, "Stamina System", "SR_FoodUse");
		detail::set_value(iniSettings, Stamina_Regen.Poison, "Stamina System", "SR_CombatOnly");
		detail::set_value(iniSettings, Stamina_Regen.BestValue, "Stamina System", "SR_BestValue");
		detail::set_value(iniSettings, Stamina_Regen.Enabled, "Stamina System", "SR_Enabled");
		detail::set_value(iniSettings, Stamina_Regen.CombatOnly, "Stamina System", "SR_CombatOnly");
		detail::set_value(iniSettings, Stamina_Regen.Threshold, "Stamina System", "SR_Threshold");

		// Fortification
		detail::set_value(iniSettings, Stamina_Fortify.EffectName, "Stamina System", "SF_EffectName");
		detail::set_value(iniSettings, Stamina_Fortify.UseFood, "Stamina System", "SF_FoodUse");
		detail::set_value(iniSettings, Stamina_Fortify.Poison, "Stamina System", "SF_CombatOnly");
		detail::set_value(iniSettings, Stamina_Fortify.BestValue, "Stamina System", "SF_BestValue");
		detail::set_value(iniSettings, Stamina_Fortify.Enabled, "Stamina System", "SF_Enabled");
		detail::set_value(iniSettings, Stamina_Fortify.CombatOnly, "Stamina System", "SF_CombatOnly");
		detail::set_value(iniSettings, Stamina_Fortify.Threshold, "Stamina System", "SF_Threshold");

		// Additional Effects
		detail::set_value(iniSettings, Cure_Disease.EffectName, "Other Auto Effects", "Disease_EffectName");
		detail::set_value(iniSettings, Cure_Disease.EffectName, "Other Auto Effects", "Disease_Enabled");
		detail::set_value(iniSettings, Cure_Poison.EffectName, "Other Auto Effects", "Poison_EffectName");
		detail::set_value(iniSettings, Cure_Poison.EffectName, "Other Auto Effects", "Poison_Enabled");

		iniSettings.SaveFile(settings_path);
	}

	void LoadHotkeys()
	{
		HotkeyRecords.clear();
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
		HotkeyRecords.push_back({});
	}

	void LoadHotkeyINI() 
	{
		constexpr auto hotkey_path = L"Data/SKSE/Plugins/SwiftPotionNG_Hotkeys.ini";

		CSimpleIniA iniHotkeys;
		iniHotkeys.SetUnicode();
		iniHotkeys.LoadFile(hotkey_path);

		int idx = 0;
		for (auto& pData : HotkeyRecords) {
			std::string iniSection = HotkeyString(idx,"Hotkey");
			detail::get_value(iniHotkeys, pData.EffectName, iniSection.c_str(), HotkeyString(idx,"EffectName").c_str());
			detail::get_value(iniHotkeys, pData.UseFood, iniSection.c_str(), HotkeyString(idx,"UseFood").c_str());
			detail::get_value(iniHotkeys, pData.Poison, iniSection.c_str(), HotkeyString(idx,"Poison").c_str());
			detail::get_value(iniHotkeys, pData.BestValue, iniSection.c_str(), HotkeyString(idx,"BestValue").c_str());
			detail::get_value(iniHotkeys, pData.Hotkey, iniSection.c_str(), HotkeyString(idx,"Hotkey").c_str());
			detail::get_value(iniHotkeys, pData.Modifier1, iniSection.c_str(), HotkeyString(idx,"Modifier1").c_str());
			detail::get_value(iniHotkeys, pData.Modifier2, iniSection.c_str(), HotkeyString(idx,"Modifier2").c_str());
			detail::get_value(iniHotkeys, pData.Modifier3, iniSection.c_str(), HotkeyString(idx,"Modifier3").c_str());
			idx++;
		}
	}

	void SaveHotkeyINI() 
	{
		constexpr auto hotkey_path = L"Data/SKSE/Plugins/SwiftPotionNG_Hotkeys.ini";

		CSimpleIniA iniHotkeys;
		iniHotkeys.SetUnicode();
		iniHotkeys.LoadFile(hotkey_path);

		int idx = 0;
		for (auto pData : HotkeyRecords) {
			std::string iniSection = HotkeyString(idx,"Hotkey");
			detail::set_value(iniHotkeys, pData.EffectName, iniSection.c_str(), HotkeyString(idx,"EffectName").c_str());
			detail::set_value(iniHotkeys, pData.UseFood, iniSection.c_str(), HotkeyString(idx,"UseFood").c_str());
			detail::set_value(iniHotkeys, pData.Poison, iniSection.c_str(), HotkeyString(idx,"Poison").c_str());
			detail::set_value(iniHotkeys, pData.BestValue, iniSection.c_str(), HotkeyString(idx,"BestValue").c_str());
			detail::set_value(iniHotkeys, pData.Hotkey, iniSection.c_str(), HotkeyString(idx,"Hotkey").c_str());
			detail::set_value(iniHotkeys, pData.Modifier1, iniSection.c_str(), HotkeyString(idx,"Modifier1").c_str());
			detail::set_value(iniHotkeys, pData.Modifier2, iniSection.c_str(), HotkeyString(idx,"Modifier2").c_str());
			detail::set_value(iniHotkeys, pData.Modifier3, iniSection.c_str(), HotkeyString(idx,"Modifier3").c_str());
			idx++;
		}

		iniHotkeys.SaveFile(hotkey_path);
	}

	std::string HotkeyString(int iNum, std::string sName)
	{
		std::string s = std::to_string(iNum);

		if (iNum < 10) {
			return "0" + s + "_" + sName;
		} else {
			return s + "_" + sName;
		}
	}

	void AddEffect(std::string sEffect, bool bNegative)
	{
		if (bNegative) {
			if (!(std::find(Negative_Effects.begin(), Negative_Effects.end(), sEffect) != Negative_Effects.end())) {
				Negative_Effects.push_back(sEffect);

				// Sort the array
				Negative_Effects.erase(std::ranges::remove_if(Negative_Effects, [](const std::string& str) { return str.empty(); }).begin(),Negative_Effects.end());
				std::ranges::sort(Negative_Effects);
			}
		} else {
			if (!(std::find(Positive_Effects.begin(), Positive_Effects.end(), sEffect) != Positive_Effects.end())) {
				Positive_Effects.push_back(sEffect);

				// Sort the array
				Positive_Effects.erase(std::ranges::remove_if(Positive_Effects, [](const std::string& str) { return str.empty(); }).begin(),Positive_Effects.end());
				std::ranges::sort(Positive_Effects);
			}
		}
	}

	void DefaultAutoData(PotionData &SystemData, std::string sType, std::string sEffectName)
	{
		// General defaults
		SystemData.EffectName = sEffectName;
		SystemData.Enabled = true;
		SystemData.CombatOnly = true;

		// Set attribute specific defaults
		if (sType.substr(0,1) == "H") {
			SystemData.Attribute = 0;
			SystemData.Hotkey = 82;
		} else if (sType.substr(0,1) == "M") {
			SystemData.Attribute = 1;
			SystemData.Hotkey = 83;
		} else if (sType.substr(0,1) == "S") {
			SystemData.Attribute = 2;
			SystemData.Hotkey = 78;
		}

		// Set effect type specific defaults
		if (sType == "H" || sType == "M" || sType == "S") {
			SystemData.BestValue = 2;
			SystemData.Threshold = 0.25;
		} else if (sType == "HR" || sType == "MR" || sType == "SR") {
			SystemData.BestValue = 0;
			SystemData.Threshold = 0.50;
		} else if (sType == "HF" || sType == "MF" || sType == "SF") {
			SystemData.BestValue = 0;
			SystemData.Threshold = 0.10;
		}

	}

	bool SPNG_Enabled { true };
	bool SPNG_Notifications { true };
	bool SPNG_HotkeyFile { false };

	int SPNG_Modifier1 { 0 };
	int SPNG_Modifier2 { 0 };
	int SPNG_Modifier3 { 0 };

	// Health Effect Names
	std::string HealthRestoreName;
    std::string HealthRegenName;
    std::string HealthFortifyName;

    // Magicka Effect Names
    std::string MagickaRestoreName;
    std::string MagickaRegenName;
    std::string MagickaFortifyName;

    // Stamina Effect Names
    std::string StaminaRestoreName;
    std::string StaminaRegenName;
    std::string StaminaFortifyName;

	// Additional Effect Names
	std::string CureDiseaseName;
	std::string CurePoisonName;

	// Potion Data
	PotionData Health_Restore;
	PotionData Health_Regen;
	PotionData Health_Fortify;

	PotionData Magicka_Restore;
	PotionData Magicka_Regen;
	PotionData Magicka_Fortify;;

	PotionData Stamina_Restore;
	PotionData Stamina_Regen;
	PotionData Stamina_Fortify;

	std::vector<PotionData> HotkeyRecords;

	// Additional Data
	PotionData Cure_Disease;
	PotionData Cure_Poison;

	// Effect Name Lists
	std::vector<std::string> Positive_Effects;
	std::vector<std::string> Negative_Effects;

	// Additional Name Lists
	std::vector<std::string> Disease_Effects;

private:
	struct detail
	{
		// Get Values
		static void get_value(CSimpleIniA& a_ini, bool& a_value, const char* a_section, const char* a_key)
		{
			if (a_ini.KeyExists(a_section, a_key)) {
				a_value = a_ini.GetBoolValue(a_section, a_key, a_value);
			} else {
				a_ini.SetBoolValue(a_section, a_key, a_value);
			}	
		};

		static void get_value(CSimpleIniA& a_ini, std::string& a_value, const char* a_section, const char* a_key) 
		{
			char const *pchar = a_value.c_str();
			if (a_ini.KeyExists(a_section, a_key)) {
				a_value = a_ini.GetValue(a_section, a_key, pchar);
			} else {
				a_ini.SetValue(a_section, a_key, pchar);
			}
		};

		static void get_value(CSimpleIniA& a_ini, int& a_value, const char* a_section, const char* a_key)
		{
			std::string s = std::to_string(a_value);
			char const *pchar = s.c_str();
			if (a_ini.KeyExists(a_section, a_key)) {
				a_value = std::stoi(a_ini.GetValue(a_section, a_key, pchar));
			} else {
				a_ini.SetValue(a_section, a_key, pchar);
			}
		};

		static void get_value(CSimpleIniA& a_ini, float& a_value, const char* a_section, const char* a_key)
		{
			std::string s = std::to_string(a_value);
			char const *pchar = s.c_str();
			if (a_ini.KeyExists(a_section, a_key)) {
				a_value = std::stof(a_ini.GetValue(a_section, a_key, pchar));
			} else {
				a_ini.SetValue(a_section, a_key, pchar);
			}
		};

		// Set Values
		static void set_value(CSimpleIniA& a_ini, bool a_value, const char* a_section, const char* a_key)
		{
			a_ini.SetBoolValue(a_section, a_key, a_value);
		};

		static void set_value(CSimpleIniA& a_ini, std::string a_value, const char* a_section, const char* a_key) 
		{
			char const *pchar = a_value.c_str();
			a_ini.SetValue(a_section, a_key, pchar);
		};

		static void set_value(CSimpleIniA& a_ini, int a_value, const char* a_section, const char* a_key)
		{
			std::string s = std::to_string(a_value);
			char const *pchar = s.c_str();
			a_ini.SetValue(a_section, a_key, pchar);
		};

		static void set_value(CSimpleIniA& a_ini, float a_value, const char* a_section, const char* a_key)
		{
			std::string s = std::to_string(a_value);
			char const *pchar = s.c_str();
			a_ini.SetValue(a_section, a_key, pchar);
		};

	};
};