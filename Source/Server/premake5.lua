project "Server"
    language "C++"
    kind "ConsoleApp"
    includedirs { 
		"./",
		"../AsioNet",
	}
	links{
		"AsioNet"
	}
    files { 
		"**.h",
		"**.cpp",
		"**.lua",
	}