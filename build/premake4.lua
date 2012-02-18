solution "crust"
    configurations { "debug", "release" }

    project "crust"
        kind "ConsoleApp"
        language "C++"
        files { "../src/**.hpp", "../src/**.cpp" }
        includedirs {
            "../src",
            "../src/**",
            "../ext/Box2D/include",
            "../ext/SDL/include"
        }
        libdirs { "../ext/Box2D/lib", "../ext/SDL/lib" }
        links { "Box2D", "SDL" }
        defines { "GL_GLEXT_PROTOTYPES" }

        configuration "debug"
           defines { "DEBUG" }
           flags { "Symbols" }
           targetdir "bin/debug"

        configuration "release"
           defines { "NDEBUG" }
           flags { "Optimize" }
           targetdir "bin/release"

        configuration "macosx"
           links { "OpenGL.framework" }
