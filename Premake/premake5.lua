workspace "Dev"
    configurations { "Debug", "Release" }
    location "../Build"
    cppdialect "C++11"
            
    filter "configurations:Debug"
        defines     "_DEBUG"
        symbols     "On"
    filter "configurations:Release"
        defines     "NDEBUG"
        optimize    "Full"
	group "ThirdParty"
		project "rebuildproject"
			language "C++"
			kind "Utility"
			prebuildcommands {"cd ../", "call make_vs_dev.bat"}
			files{
				"*.lua",
			}
    group ""
    include "../Source/AsioNet"
    include "../Source/Client"
    include "../Source/Server"