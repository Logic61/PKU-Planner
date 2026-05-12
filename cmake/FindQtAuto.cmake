# =============================================================================
# FindQtAuto.cmake - Zero Config Qt Discovery
# =============================================================================
# Scans common Qt installation paths and auto-detects Qt6.
# Priority: QT_ROOT env > C:/Qt > D:/Qt > (sorted by version desc)
# Supports: mingw_64, msvc2022_64, and other architectures.
# =============================================================================

set(_DEBUG_FIND_QT_AUTO OFF)

# -----------------------------------------------------------------------------
# 1. Candidate roots (order matters)
# -----------------------------------------------------------------------------
set(_QT_CANDIDATE_ROOTS
    "$ENV{QT_ROOT}"
    "C:/Qt"
    "D:/Qt"
    "$ENV{HOME}/Qt"
)

if(_DEBUG_FIND_QT_AUTO)
    message(STATUS "[FindQtAuto] Candidate roots: ${_QT_CANDIDATE_ROOTS}")
endif()

# -----------------------------------------------------------------------------
# 2. Optional user-specified version via QT_VERSION env var
# -----------------------------------------------------------------------------
set(_QT_PREFERRED_VERSION "$ENV{QT_VERSION}")

# -----------------------------------------------------------------------------
# 3. Version sorting comparator (sorts "6.10.0" > "6.9.0" > "6.8.0")
#    We use a simple string sort (GLOB returns lexicographic order).
#    To bias toward newer versions, we reverse the list after SORT.
# -----------------------------------------------------------------------------
macro(_list_reverse LST)
    set(_tmp)
    list(REVERSE LST)
endmacro()

# -----------------------------------------------------------------------------
# 4. Scan each root
# -----------------------------------------------------------------------------
unset(_QT6_FOUND_DIR CACHE)
unset(_WINDEPLOYQT_PATH CACHE)

foreach(_root ${_QT_CANDIDATE_ROOTS})
    if(NOT EXISTS "${_root}")
        continue()
    endif()

    if(_DEBUG_FIND_QT_AUTO)
        message(STATUS "[FindQtAuto] Scanning root: ${_root}")
    endif()

    # List all version subdirs
    file(GLOB _qt_versions LIST_DIRECTORIES true "${_root}/*")

    if(NOT _qt_versions)
        continue()
    endif()

    # Sort descending so newer versions come first
    list(SORT _qt_versions)
    list(REVERSE _qt_versions)

    foreach(_ver_path IN LISTS _qt_versions)
        get_filename_component(_ver_name "${_ver_path}" NAME)

        # Skip non-version dirs (e.g. "dist", "Docs", "Tools")
        if(NOT _ver_name MATCHES "^[0-9]")
            continue()
        endif()

        # If user specified QT_VERSION, skip non-matching versions
        if(_QT_PREFERRED_VERSION AND NOT _ver_name VERSION_EQUAL _QT_PREFERRED_VERSION)
            if(_DEBUG_FIND_QT_AUTO)
                message(STATUS "[FindQtAuto] Skipping ${_ver_name} (wanted ${_QT_PREFERRED_VERSION})")
            endif()
            continue()
        endif()

        if(_DEBUG_FIND_QT_AUTO)
            message(STATUS "[FindQtAuto] Trying version: ${_ver_name}")
        endif()

        # Architecture subdirs: mingw_64, msvc2022_64, clang_64, etc.
        # Also check the root directly (some zip installs flatten it)
        file(GLOB _arch_candidates "${_ver_path}/mingw_*" "${_ver_path}/msvc*"
             "${_ver_path}/clang_*" "${_ver_path}/*")

        foreach(_arch_path IN LISTS _arch_candidates)
            if(_DEBUG_FIND_QT_AUTO)
                message(STATUS "[FindQtAuto]   Checking arch: ${_arch_path}")
            endif()

            set(_cmake_dir "${_arch_path}/lib/cmake/Qt6")
            if(EXISTS "${_cmake_dir}/Qt6Config.cmake")
                set(_QT6_FOUND_DIR "${_cmake_dir}" CACHE PATH "Auto-detected Qt6" FORCE)

                if(_DEBUG_FIND_QT_AUTO)
                    message(STATUS "[FindQtAuto] FOUND Qt6 at: ${_QT6_FOUND_DIR}")
                endif()

                # Also auto-find windeployqt in the same arch tree
                if(NOT _WINDEPLOYQT_PATH)
                    file(GLOB _bins "${_arch_path}/bin/windeployqt*" "${_arch_path}/bin/windeployqt.exe")
                    if(_bins)
                        list(GET _bins 0 _WINDEPLOYQT_PATH)
                        if(_DEBUG_FIND_QT_AUTO)
                            message(STATUS "[FindQtAuto] Found windeployqt: ${_WINDEPLOYQT_PATH}")
                        endif()
                    endif()
                endif()

                break()  # found arch
            endif()
        endforeach()

        if(_QT6_FOUND_DIR)
            break()  # found version
        endif()
    endforeach()

    if(_QT6_FOUND_DIR)
        break()  # found root
    endif()
endforeach()

# -----------------------------------------------------------------------------
# 5. Fail with a clear message (instead of CMake's cryptic "not found")
# -----------------------------------------------------------------------------
if(NOT _QT6_FOUND_DIR)
    message(FATAL_ERROR
        "[FindQtAuto] Qt6 not found.\n"
        "\n"
        "Searched locations:\n"
        "  - QT_ROOT environment variable (if set)\n"
        "  - C:/Qt\n"
        "  - D:/Qt\n"
        "  - HOME/Qt\n"
        "\n"
        "To fix:\n"
        "  Option A: Install Qt 6 from https://www.qt.io/download-qt-installer\n"
        "  Option B: Set QT_ROOT=C:/path/to/Qt/6.x.x/arch   (e.g. C:/Qt/6.10.0/mingw_64)\n"
        "  Option C: Set QT_VERSION=6.10.0 to pin a specific version\n"
        "\n"
        "CMake will now abort."
    )
endif()

# -----------------------------------------------------------------------------
# 6. Export Qt6_DIR so find_package(Qt6 ...) picks it up automatically
# -----------------------------------------------------------------------------
set(Qt6_DIR "${_QT6_FOUND_DIR}" CACHE PATH "Qt6 CMake directory" FORCE)

if(_WINDEPLOYQT_PATH)
    set(WINDEPLOYQT_EXECUTABLE "${_WINDEPLOYQT_PATH}" CACHE FILEPATH "windeployqt path" FORCE)
    message(STATUS "[FindQtAuto] windeployqt: ${WINDEPLOYQT_EXECUTABLE}")
endif()

message(STATUS "[FindQtAuto] Qt6_DIR: ${Qt6_DIR}")
message(STATUS "[FindQtAuto] Detection complete.")

# Append Qt bin to PATH so windeployqt can find qtpaths
get_filename_component(_QT_BIN_DIR "${_WINDEPLOYQT_PATH}" DIRECTORY)
file(TO_NATIVE_PATH "${_QT_BIN_DIR}" _QT_BIN_DIR_NATIVE)
set(ENV{PATH} "${_QT_BIN_DIR_NATIVE};$ENV{PATH}")
