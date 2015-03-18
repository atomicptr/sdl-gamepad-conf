solution "sdl-gamepad-conf"
    configurations {"debug", "release"}

    language "C++"

    buildoptions {"-std=c++1y"}

    includedirs {
        "deps/ntd",
        "deps/jsoncpp/include"
    }

    project "deps"
        kind "StaticLib"

        buildoptions {"-x c++", "-std=c++1y"}

        files { "deps/**.c", "deps/**.cpp" }

        warnings "Off"

        configuration {"gmake", "windows"}
            linkoptions {"-lmingw32"}

        configuration {"gmake", "linux"}
            if _OPTIONS["cc"] == "clang" then
                toolset "clang"
                buildoptions "-stdlib=libc++"
                links "c++"
            end

    project "sdl-gamepad-conf"
        kind "WindowedApp"

        includedirs "include"

        files "src/**.cpp"

        buildoptions "-std=c++1y"

        configuration {"gmake", "windows"}
            includedirs {
                "deps/mingw/SDL2-2.0.3/x86_64-w64-mingw32/include",
                "deps/mingw/SDL2-2.0.3/x86_64-w64-mingw32/include/SDL2",
                "deps/mingw/SDL2_image-2.0.0/x86_64-w64-mingw32/include",
                "deps/mingw/SDL2_image-2.0.0/x86_64-w64-mingw32/include/SDL2"
            }

            libdirs {
                "deps/mingw/SDL2-2.0.3/x86_64-w64-mingw32/lib",
                "deps/mingw/SDL2_image-2.0.0/x86_64-w64-mingw32/lib"
            }

            linkoptions {"-lmingw32 -lSDL2main -lSDL2 -lSDL2_image"}

            links {"deps"}

            postbuildcommands {
                "cp deps/mingw/SDL2-2.0.3/x86_64-w64-mingw32/bin/SDL2.dll SDL2.dll",
                "cp deps/mingw/SDL2_image-2.0.0/x86_64-w64-mingw32/bin/*.dll ."
            }

        configuration {"gmake", "linux"}
            if _OPTIONS["cc"] == "clang" then
                toolset "clang"
                buildoptions "-stdlib=libc++"
                links "c++"
            end
            links { "SDL2", "SDL2_image", "deps" }

        configuration {"gmake", "macosx"}
            links { "SDL2", "SDL2_image","deps" }

        configuration "debug"
            flags {"Symbols"}
