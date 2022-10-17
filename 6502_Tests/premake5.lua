project "6502_Tests"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"
	systemversion "latest"

	targetdir (buildDir)
	objdir (buildObjDir)

	files {
		"Source/**.h",
		"Source/**.cpp",
	}

	includedirs {
		"%{includeDirs.Lib6502}",
		"%{includeDirs.spdlog}",
		"%{includeDirs.GoogleTest}"
	}

	links {
		"6502",
		"GoogleTest"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Distribution"
		defines { "DISTRIBUTION_6502" }
		runtime "Release"
		optimize "Full"