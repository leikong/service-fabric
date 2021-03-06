cmake_minimum_required(VERSION 3.10)

project(Horizon)

# Includes
include_directories("src") # src/prod/src/pal/src/internal/pal.cpp:16:10: fatal error: 'Common/Common.h' file not found
include_directories("src/pal/src")
include_directories("shared")
include_directories("shared/serialization/inc") # Serialization.h error: 'FabricIOBuffer.h' file not found with <angled> include; use "quotes" instead
include_directories("ktl/src/inc")
include_directories("ktl/src/linuxinc")
include_directories("ktl/src/logger/inc") # src/prod/shared/ktlwf/logger/com/dll/../../../../../src/ktllogger/sys/inc/ktllogger.h:15:10: fatal error: 'KtlPhysicalLog.h' file not found
include_directories("src/prebuilt/sdk") # FabricTypes.h
include_directories("src/prebuilt/internal") # FabricTypes_.h

# Variables
message(STATUS "CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}")
set(PROJECT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "Project binary directory")
set(LIBRARY_OUTPUT_DIR ${PROJECT_BINARY_DIR}/lib CACHE STRING "Static library output directory")
set(DLL_OUTPUT_DIR ${PROJECT_BINARY_DIR}/dll CACHE STRING "Dynamic library output directory")
set(EXECUTABLE_OUTPUT_DIR ${PROJECT_BINARY_DIR}/exe CACHE STRING "Executable output directory")

# Horizon static library names
set(lib_Common "Common" CACHE STRING "Common library")
set(lib_FabricCommon "FabricCommon" CACHE STRING "FabricCommon library")
set(lib_FabricResources "FabricResources" CACHE STRING "Fabric Resources library")
set(lib_Federation "Federation" CACHE STRING "Federation library")
set(lib_KComAdapter "KComAdapter" CACHE STRING "KComAdapter library")
set(lib_KPhysicalLog "KPhysicalLog" CACHE STRING "KPhysicalLog adapter")
set(lib_Lease "Lease" CACHE STRING "Lease library")
set(lib_LeaseAgent "LeaseAgent" CACHE STRING "LeaseAgent library")
set(lib_Serialization "Serialization" CACHE STRING "Serialization library")
set(lib_ServiceModel "ServiceModel" CACHE STRING "Service Model library")
set(lib_Transport "Transport" CACHE STRING "Transport library")

# Horizon dynamic library names
set(dll_PyHost "PyHost" CACHE STRING "Python embedded hosting library")

# Compiler flags
#
# src/pal/wc16.c compiles with Clang rather than Clang++ (compile error applying -std=c++* to Clang)
set(CMAKE_CXX_FLAGS, "${CMAKE_CXX_FLAGS} -std=c++14") 

add_compile_options(-stdlib=libc++)
add_compile_options(-fdeclspec) 
add_compile_options(-fms-extensions)
#
# SF Linux docs say that our PAL layer has hardcoded assumptions
# about wchar_t being 2 bytes, but turning this on results
# in erroneous wstring/string conversions elsewhere?
#
#add_compile_options(-fshort-wchar) # 2-byte wchar
add_compile_options(-fexceptions) # generate unwind info
add_compile_options(-fcoroutines-ts) # KTL
add_compile_options(-fdelayed-template-parsing)
add_compile_options(-fPIC) # shared/ktlwf/logger

# Disable warnings
#
# src/prod/src/Common
add_compile_options(-Wno-microsoft-extra-qualification)
add_compile_options(-Wno-microsoft-template)
add_compile_options(-Wno-microsoft-pure-definition)
add_compile_options(-Wno-mismatched-new-delete)
add_compile_options(-Wno-switch)
add_compile_options(-Wno-delete-non-virtual-dtor)
add_compile_options(-Wno-undefined-inline)
add_compile_options(-Wno-invalid-noreturn)
add_compile_options(-Wno-return-type)
add_compile_options(-Wno-deprecated-declarations)
add_compile_options(-Wno-c++17-extensions)
add_compile_options(-Wno-unsequenced)
add_compile_options(-Wno-format)
add_compile_options(-Wno-logical-not-parentheses)
add_compile_options(-Wno-dangling-else)
add_compile_options(-Wno-extra-tokens)
add_compile_options(-Wno-duplicate-decl-specifier)
add_compile_options(-Wno-logical-op-parentheses)
add_compile_options(-Wno-logical-not-parentheses)
add_compile_options(-Wno-delete-incomplete)
add_compile_options(-Wno-user-defined-warnings)
# src/prod/src/pal/src
add_compile_options(-Wno-macro-redefined)
add_compile_options(-Wno-ignored-attributes)
# src/prod/ktl
add_compile_options(-Wno-inconsistent-missing-override)
add_compile_options(-Wno-unused-value)
add_compile_options(-Wno-constant-conversion)
add_compile_options(-Wno-invalid-offsetof)
add_compile_options(-Wno-writable-strings)
add_compile_options(-Wno-int-to-void-pointer-cast)
# src/prod/shared/ktlwf/logger
add_compile_options(-Wno-enum-compare)
# src/prod/src/Transport
add_compile_options(-Wno-redeclared-class-member)
# src/prod/src/Federation
add_compile_options(-Wno-pragma-once-outside-header)
# src/prod/src/resources
add_compile_options(-Wno-implicit-function-declaration)

# Definitions
#
# PAL
add_definitions(-DPLATFORM_UNIX=1)
add_definitions(-D__LINUX__=1)
add_definitions(-DUSE_STL=1)
add_definitions(-DUSE_STDLIB=1)
add_definitions(-DUNICODE=1)
add_definitions(-DUSE_PLATFORM_MATH=1)
# PAL
add_definitions(-DLP64COMPATIBLE=1)
add_definitions(-DCORECLR=1)
add_definitions(-DPIC=1)
add_definitions(-DBIT64=1)
add_definitions(-D_WIN64=1)
add_definitions(-DPAL_STDCPP_COMPAT=1)
# KTL
add_definitions(-DKTL_USER_MODE=1)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDBG=1)
endif()

# TODO: Can these custom functions be moved into another file?
find_program(AWK awk)
if (AWK STREQUAL "AWK-NOTFOUND")
    message(FATAL_ERROR "AWK not found")
endif()

function(generate_exports_file tgt inFile outFile)
  set(AWK_SCRIPT symexports.awk)
  add_custom_command(
    TARGET ${tgt} PRE_LINK
    COMMAND ${AWK} -f ${PROJECT_SOURCE_DIR}/tools/linux/cmake/${AWK_SCRIPT} ${inFile} > ${outFile} 2>${tgt}.err
    DEPENDS ${inFile} ${PROJECT_SOURCE_DIR}/tools/linux/cmake/${AWK_SCRIPT}
    COMMENT "Generating exports file ${outFile}"
  )
  set_source_files_properties(${outFile}
                              PROPERTIES GENERATED TRUE)
endfunction()

# Subdirectories (order matters in CMakeLists.txt - add subdirectories last)
#
# build ktl first as it has its own "toplevel" CMakeLists.txt
add_subdirectory(ktl)
add_subdirectory(src)
add_subdirectory(shared)
