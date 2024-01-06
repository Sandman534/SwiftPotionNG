cmake_minimum_required(VERSION 3.21)
message("Using toolchain file ${CMAKE_TOOLCHAIN_FILE}.")

########################################################################################################################
## Define project
########################################################################################################################
project(
        SwiftPotionNG
        VERSION 1.0.0
        DESCRIPTION "A potion extension for swift potion"
        LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

include(GNUInstallDirs)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
        @ONLY)

set(HEADERS
        src/Events.h
        src/FormLoader.h
        src/Hooks.h
        src/Papyrus.h
        src/Serialization.h
        src/Settings.h
        src/SwiftPotion.h
        src/Utility.h)

set(SOURCES
        src/Main.cpp
        src/Papyrus.cpp
        src/SwiftPotion.cpp)

set(TESTS
        test/SwiftPotionExtension.cpp)

source_group(
        TREE ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
        ${HEADERS}
        ${SOURCES}
        ${TESTS})

#########################################################################################################################
### Build options
#########################################################################################################################
message("Options:")
option(BUILD_TESTS "Build unit tests." OFF)
message("\tTests: ${BUILD_TESTS}")

########################################################################################################################
## Configure target DLL
########################################################################################################################
find_package(CommonLibSSE CONFIG REQUIRED)
find_package(ryml CONFIG REQUIRED)
find_path(ARTICUNO_INCLUDE_DIRS "articuno/articuno.h")
find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")

add_commonlibsse_plugin(${PROJECT_NAME} SOURCES ${HEADERS} ${SOURCES})
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")

target_include_directories(${PROJECT_NAME}
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:src>
        ${ARTICUNO_INCLUDE_DIRS}
        ${SIMPLEINI_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ryml::ryml)

target_precompile_headers(${PROJECT_NAME}
        PRIVATE
        src/PCH.h)

install(TARGETS ${PROJECT_NAME}
        DESTINATION "${CMAKE_INSTALL_LIBDIR}")

########################################################################################################################
## Automatic plugin deployment
########################################################################################################################
# Set output folder, defaults to an MO2 Mod folder
set(OUTPUT_FOLDER "E:/ModOrganizer/Skyrim SE/mods/Swift Potion NG")

# When your SKSE .dll is compiled, this will automatically copy the .dll into your mods folder.
# Only works if you configure DEPLOY_ROOT above (or set the SKYRIM_MODS_FOLDER environment variable)
if(DEFINED OUTPUT_FOLDER)
    # If you specify an <OUTPUT_FOLDER> (including via environment variables)
    # then we'll copy your mod files into Skyrim or a mod manager for you!

    # Copy the SKSE plugin .dll files into the SKSE/Plugins/ folder
    set(DLL_FOLDER "${OUTPUT_FOLDER}/SKSE/Plugins")

    message(STATUS "SKSE plugin output folder: ${DLL_FOLDER}")

    add_custom_command(
        TARGET "${PROJECT_NAME}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${DLL_FOLDER}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        VERBATIM
    )

    # If you perform a "Debug" build, also copy .pdb file (for debug symbols)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_command(
            TARGET "${PROJECT_NAME}"
            POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
            VERBATIM
        )
    endif()
endif()
