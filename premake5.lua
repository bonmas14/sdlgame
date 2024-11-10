workspace "sdlgame"
    configurations { "Debug", "Release" }

    architecture "x86_64"

    defines { "_CRT_SECURE_NO_WARNINGS" }

    filter "configurations:Debug"
        defines { "DEBUG" }  
        symbols "On"
        optimize "Debug"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "Full"

    filter {}

project "game"
    -- kind "WindowedApp"
    kind "ConsoleApp"
    language "C"

    cdialect "c11"
    flags { "MultiProcessorCompile" }
    filter "configurations:Debug"
        targetdir "bin"
        targetsuffix "-d"
        staticruntime "on"
        runtime "Debug"
	libdirs { "./SDL/build/Debug/" }
	links { "SDL2d", "SDL2maind" }

    filter "configurations:Release"
        targetdir "bin"
        targetsuffix "-r"
        staticruntime "on"
        runtime "Release"
	libdirs { "./SDL/build/Release/" }
	links { "SDL2", "SDL2main" }

    filter {}

    includedirs { "./SDL/include" }

    files { "./src/**.c", "./src/**.h" }

