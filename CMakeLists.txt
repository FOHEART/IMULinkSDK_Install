cmake_minimum_required(VERSION 3.20)

set(TARGET_NAME "IMULinkSDK_Examples")

project(${TARGET_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to enable Python bindings (pybind11). Default is OFF.
option(BUILD_PYTHON_BINDINGS "Enable building Python bindings (requires pybind11). Default: OFF" OFF)

if(BUILD_PYTHON_BINDINGS)
  find_package(pybind11 CONFIG QUIET)
  if(NOT pybind11_FOUND)
    include(FetchContent)
    FetchContent_Declare(
      pybind11
      GIT_REPOSITORY https://github.com/pybind/pybind11.git
      GIT_TAG v2.13.6
    )
    FetchContent_MakeAvailable(pybind11)
  endif()
endif()

if(NOT WIN32)
  message(FATAL_ERROR "This CMakeLists currently supports Windows layout of out/install/*")
endif()

# SDK install layout:
#   out/install/x64-debug|x64-release/{include,lib,bin}
set(_IMULINK_INSTALL_ROOT "${CMAKE_SOURCE_DIR}/out/install")

# Derive the SDK config subdir.
# - For multi-config generators (Visual Studio), DO NOT use generator expressions for existence checks.
#   Instead decide at configure-time which actual folders exist.
# - For single-config generators (Ninja), resolve at configure time.
get_property(_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_is_multi_config)
  # Detect which on-disk folders exist. Prefer lower-case, then capitalized.
  set(_IMULINKSDK_DBG_ROOT "")
  if(EXISTS "${_IMULINK_INSTALL_ROOT}/x64-debug")
    set(_IMULINKSDK_DBG_ROOT "${_IMULINK_INSTALL_ROOT}/x64-debug")
  elseif(EXISTS "${_IMULINK_INSTALL_ROOT}/x64-Debug")
    set(_IMULINKSDK_DBG_ROOT "${_IMULINK_INSTALL_ROOT}/x64-Debug")
  endif()

  set(_IMULINKSDK_REL_ROOT "")
  if(EXISTS "${_IMULINK_INSTALL_ROOT}/x64-release")
    set(_IMULINKSDK_REL_ROOT "${_IMULINK_INSTALL_ROOT}/x64-release")
  elseif(EXISTS "${_IMULINK_INSTALL_ROOT}/x64-Release")
    set(_IMULINKSDK_REL_ROOT "${_IMULINK_INSTALL_ROOT}/x64-Release")
  endif()

  # Fallbacks if only one config is installed.
  if(NOT _IMULINKSDK_DBG_ROOT)
    set(_IMULINKSDK_DBG_ROOT "${_IMULINKSDK_REL_ROOT}")
  endif()
  if(NOT _IMULINKSDK_REL_ROOT)
    set(_IMULINKSDK_REL_ROOT "${_IMULINKSDK_DBG_ROOT}")
  endif()

  if(NOT _IMULINKSDK_DBG_ROOT OR NOT _IMULINKSDK_REL_ROOT)
    message(FATAL_ERROR "Could not find IMULinkSDK install under '${_IMULINK_INSTALL_ROOT}'. Expected x64-debug/x64-Debug and/or x64-release/x64-Release.")
  endif()

  # Provide generator-expression-aware include/lib/bin dirs (now based on concrete roots).
  set(_IMULINKSDK_INCLUDE_DIR "$<IF:$<CONFIG:Debug>,${_IMULINKSDK_DBG_ROOT},${_IMULINKSDK_REL_ROOT}>")
  set(_IMULINKSDK_LIB_DIR     "$<IF:$<CONFIG:Debug>,${_IMULINKSDK_DBG_ROOT}/lib,${_IMULINKSDK_REL_ROOT}/lib>")
  set(_IMULINKSDK_BIN_DIR     "$<IF:$<CONFIG:Debug>,${_IMULINKSDK_DBG_ROOT}/bin,${_IMULINKSDK_REL_ROOT}/bin>")
else()
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
  endif()
  string(TOLOWER "${CMAKE_BUILD_TYPE}" _bt_lc)
  if(_bt_lc STREQUAL "debug")
    set(_IMULINK_CFG_SUBDIR "x64-debug")
  else()
    set(_IMULINK_CFG_SUBDIR "x64-release")
  endif()

  # For single-config generators, prefer matching folder, then try opposite config, then try capitalized forms.
  if(EXISTS "${_IMULINK_INSTALL_ROOT}/${_IMULINK_CFG_SUBDIR}")
    set(_IMULINKSDK_INCLUDE_DIR "${_IMULINK_INSTALL_ROOT}/${_IMULINK_CFG_SUBDIR}")
    set(_IMULINKSDK_LIB_DIR     "${_IMULINK_INSTALL_ROOT}/${_IMULINK_CFG_SUBDIR}/lib")
    set(_IMULINKSDK_BIN_DIR     "${_IMULINK_INSTALL_ROOT}/${_IMULINK_CFG_SUBDIR}/bin")
  else()
    # Try the opposite config (debug vs release)
    if(_bt_lc STREQUAL "debug")
      if(EXISTS "${_IMULINK_INSTALL_ROOT}/x64-Release")
        set(_IMULINKSDK_INCLUDE_DIR "${_IMULINK_INSTALL_ROOT}/x64-Release")
        set(_IMULINKSDK_LIB_DIR     "${_IMULINK_INSTALL_ROOT}/x64-Release/lib")
        set(_IMULINKSDK_BIN_DIR     "${_IMULINK_INSTALL_ROOT}/x64-Release/bin")
      elseif(EXISTS "${_IMULINK_INSTALL_ROOT}/x64-release")
        set(_IMULINKSDK_INCLUDE_DIR "${_IMULINK_INSTALL_ROOT}/x64-release")
        set(_IMULINKSDK_LIB_DIR     "${_IMULINK_INSTALL_ROOT}/x64-release/lib")
        set(_IMULINKSDK_BIN_DIR     "${_IMULINK_INSTALL_ROOT}/x64-release/bin")
      else()
        set(_IMULINKSDK_INCLUDE_DIR "${_IMULINK_INSTALL_ROOT}/x64-Debug")
        set(_IMULINKSDK_LIB_DIR     "${_IMULINK_INSTALL_ROOT}/x64-Debug/lib")
        set(_IMULINKSDK_BIN_DIR     "${_IMULINK_INSTALL_ROOT}/x64-Debug/bin")
      endif()
    else()
      if(EXISTS "${_IMULINK_INSTALL_ROOT}/x64-debug")
        set(_IMULINKSDK_INCLUDE_DIR "${_IMULINK_INSTALL_ROOT}/x64-debug")
        set(_IMULINKSDK_LIB_DIR     "${_IMULINK_INSTALL_ROOT}/x64-debug/lib")
        set(_IMULINKSDK_BIN_DIR     "${_IMULINK_INSTALL_ROOT}/x64-debug/bin")
      elseif(EXISTS "${_IMULINK_INSTALL_ROOT}/x64-Debug")
        set(_IMULINKSDK_INCLUDE_DIR "${_IMULINK_INSTALL_ROOT}/x64-Debug")
        set(_IMULINKSDK_LIB_DIR     "${_IMULINK_INSTALL_ROOT}/x64-Debug/lib")
        set(_IMULINKSDK_BIN_DIR     "${_IMULINK_INSTALL_ROOT}/x64-Debug/bin")
      else()
        set(_IMULINKSDK_INCLUDE_DIR "${_IMULINK_INSTALL_ROOT}/x64-Release")
        set(_IMULINKSDK_LIB_DIR     "${_IMULINK_INSTALL_ROOT}/x64-Release/lib")
        set(_IMULINKSDK_BIN_DIR     "${_IMULINK_INSTALL_ROOT}/x64-Release/bin")
      endif()
    endif()
  endif()
endif()

add_library(imuLinkSDK SHARED IMPORTED GLOBAL)

# Configure imported locations per configuration for best VS compatibility.
if(_is_multi_config)
  set_target_properties(imuLinkSDK PROPERTIES
    IMPORTED_CONFIGURATIONS "Debug;Release"
    IMPORTED_IMPLIB_DEBUG "${_IMULINKSDK_DBG_ROOT}/lib/imuLinkSDK.lib"
    IMPORTED_LOCATION_DEBUG "${_IMULINKSDK_DBG_ROOT}/bin/imuLinkSDK.dll"
    IMPORTED_IMPLIB_RELEASE "${_IMULINKSDK_REL_ROOT}/lib/imuLinkSDK.lib"
    IMPORTED_LOCATION_RELEASE "${_IMULINKSDK_REL_ROOT}/bin/imuLinkSDK.dll"
    INTERFACE_INCLUDE_DIRECTORIES "${_IMULINKSDK_INCLUDE_DIR}"
  )
else()
  set_target_properties(imuLinkSDK PROPERTIES
    IMPORTED_IMPLIB "${_IMULINKSDK_LIB_DIR}/imuLinkSDK.lib"
    IMPORTED_LOCATION "${_IMULINKSDK_BIN_DIR}/imuLinkSDK.dll"
    INTERFACE_INCLUDE_DIRECTORIES "${_IMULINKSDK_INCLUDE_DIR}"
  )
endif()

# spdlog is vendored in this repository under ./spdlog
add_library(spdlog::spdlog INTERFACE IMPORTED)
set_target_properties(spdlog::spdlog PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}"
)

# Helper: copy the SDK runtime DLLs next to the built executable
function(copy_imu_runtime_dlls target)
  add_custom_command(TARGET ${target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${_IMULINKSDK_BIN_DIR}/imuLinkSDK.dll"
      "$<TARGET_FILE_DIR:${target}>/imuLinkSDK.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${_IMULINKSDK_BIN_DIR}/concrt140.dll"
      "$<TARGET_FILE_DIR:${target}>/concrt140.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${_IMULINKSDK_BIN_DIR}/msvcp140.dll"
      "$<TARGET_FILE_DIR:${target}>/msvcp140.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${_IMULINKSDK_BIN_DIR}/msvcp140_1.dll"
      "$<TARGET_FILE_DIR:${target}>/msvcp140_1.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${_IMULINKSDK_BIN_DIR}/msvcp140_2.dll"
      "$<TARGET_FILE_DIR:${target}>/msvcp140_2.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${_IMULINKSDK_BIN_DIR}/msvcp140_atomic_wait.dll"
      "$<TARGET_FILE_DIR:${target}>/msvcp140_atomic_wait.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${_IMULINKSDK_BIN_DIR}/msvcp140_codecvt_ids.dll"
      "$<TARGET_FILE_DIR:${target}>/msvcp140_codecvt_ids.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${_IMULINKSDK_BIN_DIR}/vcruntime140.dll"
      "$<TARGET_FILE_DIR:${target}>/vcruntime140.dll"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${_IMULINKSDK_BIN_DIR}/vcruntime140_1.dll"
      "$<TARGET_FILE_DIR:${target}>/vcruntime140_1.dll"
  )
endfunction()

if(BUILD_PYTHON_BINDINGS)
  pybind11_add_module(imulink_sdk_pybind MODULE
    "${CMAKE_SOURCE_DIR}/python/imulink_sdk_pybind.cpp"
  )

  target_include_directories(imulink_sdk_pybind PRIVATE
    "${CMAKE_SOURCE_DIR}"
  )
  target_link_libraries(imulink_sdk_pybind PRIVATE imuLinkSDK)

  if(MSVC)
    target_compile_options(imulink_sdk_pybind PRIVATE /utf-8)
    target_compile_definitions(imulink_sdk_pybind PRIVATE _CRT_SECURE_NO_WARNINGS)
  else()
    target_compile_options(imulink_sdk_pybind PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
  endif()

  set_target_properties(imulink_sdk_pybind PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/python"
  )

  if(_is_multi_config)
    foreach(_cfg Debug Release RelWithDebInfo MinSizeRel)
      string(TOUPPER "${_cfg}" _cfg_uc)
      set_target_properties(imulink_sdk_pybind PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY_${_cfg_uc} "${CMAKE_BINARY_DIR}/python/${_cfg}"
        RUNTIME_OUTPUT_DIRECTORY_${_cfg_uc} "${CMAKE_BINARY_DIR}/python/${_cfg}"
      )
    endforeach()
  endif()

  copy_imu_runtime_dlls(imulink_sdk_pybind)
else()
  message(STATUS "BUILD_PYTHON_BINDINGS is OFF — skipping pybind11/python module")
endif()

# Build each source file named example*.cpp as a separate exe
file(GLOB _example_cpp CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/example/example*.cpp")

foreach(_src IN LISTS _example_cpp)
  get_filename_component(_name "${_src}" NAME_WE)

  add_executable(${_name} "${_src}")

  # Allow includes like "include/IMULinkSDK.h" used by existing examples.
  target_include_directories(${_name} PRIVATE
    "${CMAKE_SOURCE_DIR}/example"
    "${CMAKE_SOURCE_DIR}"
  )

  # Link to imuLinkSDK when used (safe even if not referenced by the TU)
  target_link_libraries(${_name} PRIVATE imuLinkSDK)

  # Ensure source files are interpreted as UTF-8 by the compiler

# - MSVC: use /utf-8
# - GCC/Clang: use -finput-charset and -fexec-charset
if (MSVC)
  # Add MSVC-specific macro to suppress CRT secure warnings
  target_compile_options(${_name} PRIVATE /utf-8)
  target_compile_definitions(${_name} PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
  target_compile_options(${_name} PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

  # Logger/spdlog is used by example2
  if(_name STREQUAL "example2")
    target_sources(${_name} PRIVATE
      "${CMAKE_SOURCE_DIR}/example/Logger.cpp"
    )
    target_link_libraries(${_name} PRIVATE spdlog::spdlog)
  endif()

  copy_imu_runtime_dlls(${_name})
endforeach()
