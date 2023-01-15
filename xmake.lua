add_requires("vcpkg::cityhash", {alias = "cityhash"})

target("InternedString")
    add_rules("mode.debug", "mode.release")
    set_kind("binary")
    set_languages("cxx17")
    add_files("Source/**.cpp")
    add_headerfiles(("Source/**.hpp"))
    add_packages("cityhash")