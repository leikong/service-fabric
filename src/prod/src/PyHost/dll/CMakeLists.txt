set( SOURCES
    ../Host.cpp
    ../PyHostConfig.cpp
    ../PyInterpreter.cpp
    ../PyModuleCache.cpp
    ../PyUtils.cpp
    ../ScopedPyObject.cpp
    ./dll.cpp
)

include_directories("/usr/include/python3.6m")

add_library(objects_PyHost OBJECT ${SOURCES})
add_library(${dll_PyHost} SHARED $<TARGET_OBJECTS:objects_PyHost>)

set(EXPORTS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/exports)
set_target_properties(${dll_PyHost} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${DLL_OUTPUT_DIR})
set_target_properties(${dll_PyHost} PROPERTIES LINK_FLAGS "-Wl,-Bsymbolic -Wl,--version-script=${EXPORTS_FILE}")
