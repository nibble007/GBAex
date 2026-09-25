add_rules("mode.debug", "mode.release")

target("GBAex")
    set_kind("binary")
    add_files("GBAex/src/*.cpp")
