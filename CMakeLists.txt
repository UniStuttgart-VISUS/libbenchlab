# CMakeLists.txt
# Copyright © 2024 - 2025 Visualisierungsinstitut der Universität Stuttgart. Alle Rechte vorbehalten.
# Licensed under the MIT licence. See LICENCE file in the project root for detailed information.

cmake_minimum_required(VERSION 3.18.0)
project(benchlab)

include(CMakeDependentOption)
include(FetchContent)


# User-configurable options
#option(BENCHLAB_BuildTests "Build the test driver" OFF)
option(BENCHLAB_BuildCclient "Build the C-style test client" ON)
option(BENCHLAB_BuildCppClient "Build the C++ test client" ON)
cmake_dependent_option(BENCHLAB_BuildExcellentBenchlab "Build the excellent demo programme" ON WIN32 OFF)
#cmake_dependent_option(POWENETICS_UseUdev "Use libudev to enumerate serial devices" OFF UNIX OFF)


# Windows Implementation Library is required on Windows to discover the Benchlab
# devices from the registry.
if (WIN32)
    FetchContent_Declare(wil
        URL "https://github.com/microsoft/wil/archive/refs/tags/v1.0.240803.1.zip"
        DOWNLOAD_EXTRACT_TIMESTAMP ON
    )
    option(FAST_BUILD "" OFF)
    option(WIL_BUILD_PACKAGING "" OFF)
    option(WIL_BUILD_TESTS "" OFF)
    FetchContent_GetProperties(wil)
    if (NOT wil_POPULATED)
        FetchContent_Populate(wil)
        add_subdirectory(${wil_SOURCE_DIR} ${wil_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif ()
    mark_as_advanced(FORCE
        FETCHCONTENT_SOURCE_DIR_WIL
        FETCHCONTENT_UPDATES_DISCONNECTED_WIL
        FAST_BUILD
        WIL_BUILD_PACKAGING
        WIL_BUILD_TESTS)
endif ()


# Global compiler options, which are derived from the settings above.
add_compile_definitions(UNICODE _UNICODE)
if (BENCHLAB_BuildTests)
    add_compile_definitions(LIBBENCHLAB_EXPOSE_TO_TESTING)
endif ()


# Build the library itself.
add_subdirectory(libbenchlab)


# Build the test driver if possible. The check for that is a first-class hack
# adapted from https://jslav.livejournal.com/13059.html to get  access to the VC
# installation directory.
string(TOLOWER ${CMAKE_MAKE_PROGRAM} MakeProgramme)
if ("${MakeProgramme}" MATCHES "/msbuild.+")
    string(REGEX REPLACE "/msbuild.+" "/VC" VcInstallDir ${MakeProgramme})
    set(BenchlabCanBuildTests TRUE)
else ()
    set(BenchlabCanBuildTests FALSE)
endif ()

if (BenchlabCanBuildTests AND BENCHLAB_BuildTests)
    add_subdirectory(tests)
endif ()


# Build the test for the C API.
if (BENCHLAB_BuildCclient)
    add_subdirectory(cclient)
endif()

# Build the test for the C++ API.
if (BENCHLAB_BuildCppClient)
    add_subdirectory(cppclient)
endif()


# Build the demo programme writing to Excel.
if (BENCHLAB_BuildExcellentBenchlab)
    add_subdirectory(excellentbenchlab)
endif ()
