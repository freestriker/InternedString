add_requires("vcpkg::cityhash", {alias = "cityhash"})

target("InternedString")
    set_kind("static")
    add_files("**.cpp")
    add_headerfiles(("**.hpp"))
    add_packages("cityhash")