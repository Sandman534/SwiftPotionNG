#pragma once

#include "Settings.h"
#include "Utility.h"

namespace Serialization
{
	static constexpr std::uint32_t SerializationVersion = 1;
	static constexpr std::uint32_t ID = 'SPNG';
	static constexpr std::uint32_t SerializationType = 'SPNV';

	inline void SaveCallback(SKSE::SerializationInterface* a_skse)
	{
		auto settings = Settings::GetSingleton();
		auto utility = Utility::GetSingleton();

		if (settings->SPNG_HotkeyFile) {
			logger::error("Hotkeys Persistance turned on. Settings will not be saved per character");
			return;
		} else if (!a_skse->OpenRecord(SerializationType, SerializationVersion)) {
			logger::error("Failed to open hotkey values record");
			return;
		} else {

			// Setup hotkey string for serialization
			std::string elem = "";
			for (auto hotkeyData : settings->HotkeyRecords) {
				elem += static_cast<std::string>(hotkeyData.Serialize()) + "|";
			}

			// Serialize hotkey code data
			std::size_t size = elem.length();
			if (!a_skse->WriteRecordData(size)) {
				logger::error("Failed to write size of record data!");
				return;
			} else {
				if (!a_skse->WriteRecordData(elem.data(), static_cast<uint32_t>(elem.length()))) {
					logger::error("Failed to write element!");
					return;
				}
				logger::debug("Serialized Effect Hotkeys");
			}
		}
	}

	inline void LoadCallback(SKSE::SerializationInterface* a_skse)
	{
		auto settings = Settings::GetSingleton();

		std::uint32_t type;
		std::uint32_t version;
		std::uint32_t length;
		a_skse->GetNextRecordInfo(type, version, length);

		if (type != SerializationType) {
			logger::error("Unable to load data");
			return;
		}
			
		if (version != SerializationVersion) {
			logger::error("Unable to load data");
			return;
		}

		if (settings->SPNG_HotkeyFile) {
			logger::error("Hotkeys Persistance turned on. Settings will not be saved per character");
			return;
		}

		std::size_t size;
		if (!a_skse->ReadRecordData(size)) {
			logger::error("Failed to load size!");
			return;
		}

		std::string elem;
		elem.resize(size);

		if (!a_skse->ReadRecordData(elem.data(), static_cast<uint32_t>(size))) {
			logger::error("Failed to load element!");
			return;
		} else {
			logger::debug("Deserialized Effect Hotkeys");

			// Variables for traversing
			size_t pos = 0;
			std::string token;
			std::string delimiter = "|";
			int idx = 0;

			// Process the saved string
			while ((pos = elem.find(delimiter)) != std::string::npos) {
				token = elem.substr(0, pos);

				// Deserialize the strink
				settings->HotkeyRecords[idx].Deserialize(token);
				logger::debug(FMT_STRING("Loaded Effect Hotkey {}"), idx);

				// Erase current element and move on to the next
				elem.erase(0, pos + delimiter.length());
				idx++;
			}
		}
	}

	inline void RevertCallback([[maybe_unused]] SKSE::SerializationInterface* a_skse)
	{
		auto settings = Settings::GetSingleton();

		// With persistence we will not reset existing hotkeys
		if (settings->SPNG_HotkeyFile) {
			logger::error("Hotkeys Persistance turned on. Settings will not be saved per character");
			return;
		}

		// Clear Hotkeys
		settings->LoadHotkeys();
		logger::debug("Cleared All Hotkeys");
	}
}