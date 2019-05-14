project "AsioNet"
    language "C++"
    kind "StaticLib"
    includedirs { 
		"./",
		"../../asio",
	}
    files { 
		"**.h",
		"**.cpp",
		"**.c",
		"**.lua",
	}