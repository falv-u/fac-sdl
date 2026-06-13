-- =============================================================================
-- CONFIGURACIÓN GLOBAL DEL PROYECTO
-- =============================================================================
set_project("fac")
set_languages("c11")

add_rules("mode.debug", "mode.release")

target("game")
    set_kind("binary")
    add_includedirs("include")
    
    add_files("src/*.c")

    if is_plat("freebsd") or is_plat("bsd") then
        add_sysincludedirs("/usr/local/include", "/usr/local/include/SDL2")
        add_linkdirs("/usr/local/lib")
        set_targetdir("freebsd")
        add_links("SDL2", "SDL2_ttf", "SDL2_image")
    end

    if is_plat("linux") then
        add_requires("libsdl2",       {system = true, configs = {sysconfig = {pc = "sdl2"}}})
        add_requires("libsdl2_ttf",   {system = true, configs = {sysconfig = {pc = "SDL2_ttf"}}})
        add_requires("libsdl2_image", {system = true, configs = {sysconfig = {pc = "SDL2_image"}}})
        
        add_packages("libsdl2", "libsdl2_ttf", "libsdl2_image")

        set_targetdir("linux")
    end

    if is_plat("linux", "freebsd", "bsd") then
        add_cflags("-Wall", "-Wextra", {tools = "clang"})

        if is_mode("debug") then
            add_cflags("-g", "-O0", {tools = "clang"})
            
            add_cflags("-fsanitize=address,undefined", {tools = "clang"})
            add_ldflags("-fsanitize=address,undefined", {tools = "clang"})
            
            set_policy("build.sanitizer.address", true)
            set_policy("build.sanitizer.undefined", true)
        end
    end

after_config(function ()
    import("core.project.project")
    project.export("compile_commands")
end)
