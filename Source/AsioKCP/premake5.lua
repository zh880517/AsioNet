project "AsioKCP"
    language "C++"
    kind "StaticLib"
    includedirs { 
		".",
		"../../asio",
	}
    files { 
		"**.h",
		"**.cpp",
		"**.hpp",
		"**.lua",
	}