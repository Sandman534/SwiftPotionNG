#pragma once

#include "SwiftPotion.h"

namespace Hooks {
    inline static REL::Relocation<std::uintptr_t> On_Update_Hook{ REL::RelocationID(35565, 36564), REL::Relocate(0x748, 0xC26) };

    inline void Install() {
        SwiftPotion::InstallUpdateHook();
    }
}  // namespace Hooks