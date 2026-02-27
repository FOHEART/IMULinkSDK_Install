cmake_minimum_required(VERSION 3.20)

set(TARGET_NAME "IMULinkSDK_Examples")

project(${TARGET_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  add_compile_options(/utf-8)
endif()

if(NOT WIN32)
  message(FATAL_ERROR "This CMakeLists currently supports Windows layout of out/install/*")
endif()

# SDK install layout:
#   out/install/x64-debug|x64-release/{include,lib,bin}
set(_IMULINK_INSTALL_ROOT "${CMAKE_SOURCE_DIR}/out/install")

# Derive the SDK config subdir.
# - For multi-config generators (Visual Studio), use generator expressions.
# - For single-config generators (Ninja), resolve at configure time.
get_property(_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_is_multi_config)
  set(_IMULINK_CFG_SUBDIR "$<IF:$<CONFIG:Debug>,x64-debug,x64-release>")
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
endif()

# IMPORTANT: For multi-config generators, do not fully materialize paths that
# contain generator expressions in normal variables at configure time.
set(_IMULINKSDK_INCLUDE_DIR "${_IMULINK_INSTALL_ROOT}/${_IMULINK_CFG_SUBDIR}")
set(_IMULINKSDK_LIB_DIR     "${_IMULINK_INSTALL_ROOT}/${_IMULINK_CFG_SUBDIR}/lib")
set(_IMULINKSDK_BIN_DIR     "${_IMULINK_INSTALL_ROOT}/${_IMULINK_CFG_SUBDIR}/bin")

add_library(imuLinkSDK SHARED IMPORTED GLOBAL)

# Configure imported locations per configuration for best VS compatibility.
if(_is_multi_config)
  set(_IMULINKSDK_CFG_DBG "${_IMULINK_INSTALL_ROOT}/x64-debug")
  set(_IMULINKSDK_CFG_REL "${_IMULINK_INSTALL_ROOT}/x64-release")

  set_target_properties(imuLinkSDK PROPERTIES
    IMPORTED_CONFIGURATIONS "Debug;Release"
    IMPORTED_IMPLIB_DEBUG "${_IMULINKSDK_CFG_DBG}/lib/imuLinkSDK.lib"
    IMPORTED_LOCATION_DEBUG "${_IMULINKSDK_CFG_DBG}/bin/imuLinkSDK.dll"
    IMPORTED_IMPLIB_RELEASE "${_IMULINKSDK_CFG_REL}/lib/imuLinkSDK.lib"
    IMPORTED_LOCATION_RELEASE "${_IMULINKSDK_CFG_REL}/bin/imuLinkSDK.dll"
    INTERFACE_INCLUDE_DIRECTORIES "${_IMULINK_INSTALL_ROOT}/$<IF:$<CONFIG:Debug>,x64-debug,x64-release>"
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
