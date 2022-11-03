project "6502_GUI"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"
	systemversion "latest"

	targetdir (buildDir)
	objdir (buildObjDir)

	files {
		"Source/**.h",
		"Source/**.cpp"
	}

	includedirs {
		"%{includeDirs.Lib6502}",
		"%{includeDirs.spdlog}",
		"%{includeDirs.SFML}"
	}

	libdirs {
		"%{wks.location}/Vendor/SFML/lib"
	}

	links {
		"6502"
	}

	postbuildcommands	{
		("{COPY} %{wks.location}Vendor/SFML/bin/openal32.dll %{cfg.buildtarget.directory}")
	}


	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		links {
			"sfml-system-d.lib",
			"sfml-window-d.lib",
			"sfml-graphics-d.lib",
			"sfml-network-d.lib",
			"sfml-audio-d.lib"
		}
		postbuildcommands	{
			("{COPY} %{wks.location}Vendor/SFML/bin/sfml-audio-d-2.dll %{cfg.buildtarget.directory}"),
			("{COPY} %{wks.location}Vendor/SFML/bin/sfml-graphics-d-2.dll %{cfg.buildtarget.directory}"),
			("{COPY} %{wks.location}Vendor/SFML/bin/sfml-network-d-2.dll %{cfg.buildtarget.directory}"),
			("{COPY} %{wks.location}Vendor/SFML/bin/sfml-system-d-2.dll %{cfg.buildtarget.directory}"),
			("{COPY} %{wks.location}Vendor/SFML/bin/sfml-window-d-2.dll %{cfg.buildtarget.directory}")
		}
	
	filter "configurations:Release or configurations:Distribution"
		runtime "Release"
		links {
			"sfml-system.lib",
			"sfml-window.lib",
			"sfml-graphics.lib",
			"sfml-network.lib",
			"sfml-audio.lib"
		}
		postbuildcommands	{
			("{COPY} %{wks.location}Vendor/SFML/bin/sfml-audio-2.dll %{cfg.buildtarget.directory}"),
			("{COPY} %{wks.location}Vendor/SFML/bin/sfml-graphics-2.dll %{cfg.buildtarget.directory}"),
			("{COPY} %{wks.location}Vendor/SFML/bin/sfml-network-2.dll %{cfg.buildtarget.directory}"),
			("{COPY} %{wks.location}Vendor/SFML/bin/sfml-system-2.dll %{cfg.buildtarget.directory}"),
			("{COPY} %{wks.location}Vendor/SFML/bin/sfml-window-2.dll %{cfg.buildtarget.directory}")
		}

	filter "configurations:Release"
		optimize "On"
		
	filter "configurations:Distribution"
		defines { "DISTRIBUTION_6502" }
		optimize "Full"