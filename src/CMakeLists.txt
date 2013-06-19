find_package(Threads)

add_library(provider-power-emu SHARED
  power.cpp
  mirror.cpp
  )

target_link_libraries(provider-power-emu
  ${CMAKE_THREAD_LIBS_INIT}
  ${STATEFS_LIBRARIES}
  )

install(TARGETS provider-power-emu DESTINATION ${DST_LIB}/statefs)
