add_executable(sandbox
    main.cpp
    src/Game.cpp
    src/MainMenu.cpp
    src/Sandbox.cpp
)

target_link_libraries(sandbox
    engine
)

target_include_directories(sandbox PRIVATE
    include/
)
