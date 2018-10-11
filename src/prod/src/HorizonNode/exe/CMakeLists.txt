include_directories(".")

set(exe_Horizon "Horizon.exe" CACHE STRING "Horizon node executable")

add_executable(${exe_Horizon} ../Main.cpp)

set_target_properties(${exe_Horizon} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_DIR}) 
set_target_properties(${exe_Horizon} PROPERTIES LINK_FLAGS "-stdlib=libc++")

target_link_libraries(${exe_Horizon}
  ${dll_PyHost}
  ${lib_Federation}
  ${lib_LeaseAgent}
  ${lib_Lease}
  ${lib_Serialization}
  ${lib_Common}
  ${lib_Transport}
  ${lib_Pal}
  ${lib_FabricCommon}
# link lib_Common again after lib_Pal to fix:
# lib/libPal.a(pal.cpp.o): In function `ForkExec(char const*, char const*, char const*, char const*, int*, int*)':
# pal.cpp:(.text+0x38ca): undefined reference to `Common::ProcessWait::Setup()'
  ${lib_Common}
  ${lib_Pal}
  ${lib_ServiceModel}
  ${lib_ktlfull}
  ${lib_ktlcore}
  ${lib_KtlLttng}
  python3.6m
  rt
  ssh2
  ssl
  crypto
  pthread
  dl
  m
  uuid
)
