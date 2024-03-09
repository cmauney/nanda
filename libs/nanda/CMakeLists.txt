cmake_minimum_required(VERSION 3.25)

# ---- Declare library ----

add_library(nanda INTERFACE)

target_sources(nanda
  INTERFACE
    include/nanda/ndarray.hh
)

target_include_directories(nanda
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(nanda
  INTERFACE
    cxx_std_17
  )

target_link_libraries(nanda
  INTERFACE
    fmt::fmt
)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

