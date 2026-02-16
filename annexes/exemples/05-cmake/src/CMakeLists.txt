add_executable(mon_programme
    main.c
    utils.c
)

target_link_libraries(mon_programme
    PRIVATE ma_lib
    PRIVATE m  # libm (math)
)

target_compile_options(mon_programme PRIVATE
    -Wall
    -Wextra
    -Werror
)

install(TARGETS mon_programme
    DESTINATION bin
)
