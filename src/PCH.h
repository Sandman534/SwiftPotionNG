#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>
#include <REL/Relocation.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

// Compatible declarations with other sample projects.
#define DLLEXPORT __declspec(dllexport)

using namespace std::literals;

namespace logger = SKSE::log;

namespace util {
    using SKSE::stl::report_and_fail;
}
