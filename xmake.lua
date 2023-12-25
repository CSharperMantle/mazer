if is_plat("windows") then 
    print("Windows support is added in a fork. See https://github.com/CSharperMantle/mazer_win")
end

set_allowedplats("linux", "cygwin", "macosx")

add_rules("mode.debug", "mode.release")

add_requires("ncurses ~6.3")

set_languages("c17")
set_warnings("all", "extra", "pedantic")

if is_mode("debug") then
    set_optimize("none")
    set_symbols("debug", "hidden")
    set_policy("build.sanitizer.address", true)
    set_policy("build.sanitizer.undefined", true)
else
    set_optimize("fastest")
    set_symbols("hidden")
    set_strip("all")
end

target("mazer")
    set_kind("binary")
    add_includedirs("include")
    add_files("src/*.c")
    add_packages("ncurses")
