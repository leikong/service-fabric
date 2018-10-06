include_directories(".")

set(exe_Federation "FederationRuntime.exe" CACHE STRING "Federation executable")

add_executable(${exe_Federation} Main.cpp)

set_target_properties(${exe_Federation} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_DIR}) 
set_target_properties(${exe_Federation} PROPERTIES LINK_FLAGS "-stdlib=libc++")

target_link_libraries(${exe_Federation}
  ${lib_Federation}
  ${lib_LeaseAgent}
  ${lib_Lease}
  ${lib_Transport}
  ${lib_Serialization}
  ${lib_Common}
  ${lib_Pal}
  ${lib_ServiceModel}
  ${lib_ktlcore}
  ${lib_ktlfull}
  rt # undefined symbol timer_settime
  ssh2
  ssl
  crypto
  pthread
  dl
  m
  uuid
)
