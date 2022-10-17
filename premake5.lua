workspace "6502"
	architecture "x86_64"
	startproject "6502_Runtime"
	configurations { "Debug", "Release", "Distribution" }

outputDir = "%{cfg.buildcfg}-%{cfg.architecture}"
buildDir = "%{wks.location}/bin/" .. outputDir .. "/%{prj.name}"
buildObjDir = "%{wks.location}/bin/obj/" .. outputDir .. "/%{prj.name}"

includeDirs = {}
includeDirs["Lib6502"] = "%{wks.location}/6502/Source"
includeDirs["spdlog"] = "%{wks.location}/Vendor/spdlog/include"
includeDirs["GoogleTest"] = "%{wks.location}/Vendor/GoogleTest/googletest/include"

group "Dependencies"
	include "Vendor/GoogleTest"
group ""

include "6502"
include "6502_Runtime"
include "6502_Tests"