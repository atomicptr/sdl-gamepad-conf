solution "sdl-gamepad-conf"
    configurations {"debug", "release"}

    language "C++"

    buildoptions {"-std=c++1y"}

    includedirs "deps/include"

    project "deps"
        kind "StaticLib"

        files "deps/src/**.cpp"

    project "sdl-gamepad-conf"
        kind "WindowedApp"

        includedirs "include"

        files "src/**.cpp"

        links {"SDL2", "SDL2_image", "ntd", "deps"}

        configuration "debug"
            flags {"Symbols"}

