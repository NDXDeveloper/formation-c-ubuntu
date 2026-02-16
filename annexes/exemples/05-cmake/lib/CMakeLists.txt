add_library(ma_lib STATIC
    ma_lib.c
)

target_include_directories(ma_lib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

install(TARGETS ma_lib
    ARCHIVE DESTINATION lib
)
