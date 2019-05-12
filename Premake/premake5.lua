project "rebuildproject"
    language "C++"
    kind "Utility"
    prebuildcommands {"cd ../", "call make_vs_dev.bat"}
    files{
        "*.lua",
    }