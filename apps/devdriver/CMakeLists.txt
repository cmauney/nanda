add_executable(devdriver)

target_sources(devdriver
    PRIVATE
        src/main.cc
)

target_link_libraries(devdriver
    PRIVATE
        nanda
)
