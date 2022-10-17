project "GoogleTest"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"
	systemversion "latest"

	targetdir (buildDir)
	objdir (buildObjDir)

	includedirs {
		"googletest",
		"googletest/include"
	}

	files {
		"googletest/src/**.h",
		"googletest/src/**.cc"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Distribution"
		runtime "Release"
		optimize "Full"