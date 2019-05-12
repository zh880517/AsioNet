
workspace "AsioNet"
    configurations { "Debug", "Release" }
    location "../Build"
    cppdialect "C++11"
            
    filter "configurations:Debug"
        defines     "_DEBUG"
        symbols     "On"
    filter "configurations:Release"
        defines     "NDEBUG"
        optimize    "Full"

    include "../Source/AsioKCP"