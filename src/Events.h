#pragma once
#include "Utility.h"
#include "Settings.h"
#include "SwiftPotion.h"

namespace Events {

    // Look for changes in player inventory
    class OnContainerChangedEventHandler : public RE::BSTEventSink<RE::TESContainerChangedEvent> {
    public:
        std::mutex container_mutex;
        
        static OnContainerChangedEventHandler* GetSingleton() {
            static OnContainerChangedEventHandler singleton;
            return &singleton;
        }

        RE::BSEventNotifyControl ProcessEvent(const RE::TESContainerChangedEvent* a_event, [[maybe_unused]] RE::BSTEventSource<RE::TESContainerChangedEvent>* a_eventSource) override {
            auto utility = Utility::GetSingleton();
            
            if (a_event->newContainer != utility->GetPlayer()->formID) {
                return RE::BSEventNotifyControl::kContinue;
            }

            const std::lock_guard<std::mutex> lock(container_mutex);

            auto alchemyItem = RE::TESForm::LookupByID<RE::AlchemyItem>(a_event->baseObj);

            if (alchemyItem && !alchemyItem->IsFood()) {
                utility->StopperCheck(alchemyItem);
            }

            return RE::BSEventNotifyControl::kContinue;
        }

        static void Register() {
            RE::ScriptEventSourceHolder* eventHolder = RE::ScriptEventSourceHolder::GetSingleton();
            eventHolder->AddEventSink(OnContainerChangedEventHandler::GetSingleton());
        }
    };

    // Look for key inputs
    class InputEventHandler : public RE::BSTEventSink<RE::InputEvent*> {
    private:
        bool isModifier1{false};
        bool isModifier2{false};
        bool isModifier3{false};

    public:
        static InputEventHandler* GetSingleton() {
            static InputEventHandler singleton;
            return &singleton;
        }

        RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event,RE::BSTEventSource<RE::InputEvent*>* a_eventSource) {
            if (a_event) {
                const auto ui = RE::UI::GetSingleton();
                const auto settings = Settings::GetSingleton();

                if (ui && !ui->GameIsPaused() && !ui->IsApplicationMenuOpen() && !ui->IsItemMenuOpen() && !ui->IsMenuOpen(RE::InterfaceStrings::GetSingleton()->dialogueMenu)) {
                    const auto controlMap = RE::ControlMap::GetSingleton();
                    const auto playerCharacter = RE::PlayerCharacter::GetSingleton();
                    const auto playerControls = RE::PlayerControls::GetSingleton();

                    if (controlMap && playerCharacter && playerControls) {
                        auto event = *a_event;

                        for (auto event = *a_event; event; event = event->next) {
                            if (event->eventType == RE::INPUT_EVENT_TYPE::kButton) {
                                const auto button = static_cast<RE::ButtonEvent*>(event);
                                if (!button || (button->IsPressed() && !button->IsDown())) continue;

                                auto device = button->device.get();
                                auto scan_code = button->GetIDCode();

                                if (device == RE::INPUT_DEVICE::kMouse) {
                                    scan_code += 257;
                                } else if (device == RE::INPUT_DEVICE::kGamepad) {
                                    RE::BSWin32GamepadDevice::Key gamepadKey =
                                        static_cast<RE::BSWin32GamepadDevice::Key>(scan_code);
                                    switch (gamepadKey) {
                                        case RE::BSWin32GamepadDevice::Key::kUp:
                                            scan_code = 266;
                                            break;
                                        case RE::BSWin32GamepadDevice::Key::kDown:
                                            scan_code = 267;
                                            break;
                                        case RE::BSWin32GamepadDevice::Key::kLeft:
                                            scan_code = 268;
                                            break;
                                        case RE::BSWin32GamepadDevice::Key::kRight:
                                            scan_code = 269;
                                            break;
                                        case RE::BSWin32GamepadDevice::Key::kStart:
                                            scan_code = 270;
                                            break;
                                        case RE::BSWin32GamepadDevice::Key::kBack:
                                            scan_code = 271;
                                            break;
                                        case RE::BSWin32GamepadDevice::Key::kLeftThumb:
                                            scan_code = 272;
                                            break;
                                        case RE::BSWin32GamepadDevice::Key::kRightThumb:
                                            scan_code = 273;
                                            break;
                                        case RE::BSWin32GamepadDevice::Key::kLeftShoulder:
                                            scan_code = 274;
                                            break;
                                        case RE::BSWin32GamepadDevice::Key::kRightShoulder:
                                            scan_code = 275;
                                            break;
                                        case RE::BSWin32GamepadDevice::Key::kA:
                                            scan_code = 276;
                                            break;
                                        case RE::BSWin32GamepadDevice::Key::kB:
                                            scan_code = 277;
                                            break;
                                        case RE::BSWin32GamepadDevice::Key::kX:
                                            scan_code = 278;
                                            break;
                                        case RE::BSWin32GamepadDevice::Key::kY:
                                            scan_code = 279;
                                            break;
                                        case RE::BSWin32GamepadDevice::Key::kLeftTrigger:
                                            scan_code = 280;
                                            break;
                                        case RE::BSWin32GamepadDevice::Key::kRightTrigger:
                                            scan_code = 281;
                                            break;
                                        default:
                                            scan_code = static_cast<uint32_t>(-1);
                                            break;
                                    }
                                }

                                // Get settings
                                auto settings = Settings::GetSingleton();
                                if (!settings) return RE::BSEventNotifyControl::kContinue;

                                // Modifier Key
                                if (device == RE::INPUT_DEVICE::kKeyboard || device == RE::INPUT_DEVICE::kGamepad) {
                                    if (scan_code == settings->SPNG_Modifier1) {
                                        isModifier1 = button->IsPressed();
                                    } else if (scan_code == settings->SPNG_Modifier2) {
                                        isModifier2 = button->IsPressed();
                                    } else if (scan_code == settings->SPNG_Modifier3) {
                                        isModifier3 = button->IsPressed();
                                    }
                                }


                                if ((device == RE::INPUT_DEVICE::kKeyboard || device == RE::INPUT_DEVICE::kGamepad) && !button->IsUp()) {
                                    SwiftPotion::ProcessHotkey(scan_code, isModifier1, isModifier2, isModifier3);
                                }
                                
                            }
                        }
                    }
                }
            }
            
            return RE::BSEventNotifyControl::kContinue;
        }

        static void Register() {
            RE::BSInputDeviceManager* inputDeviceManager = RE::BSInputDeviceManager::GetSingleton();
            inputDeviceManager->AddEventSink(InputEventHandler::GetSingleton());
        }
    };

    inline static void Register() {
        OnContainerChangedEventHandler::Register();
    }

    inline static void RegisterInput() {
        InputEventHandler::Register();
    }
}
