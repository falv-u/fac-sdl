set_project("fac") 
set_languages("c11")

add_requires("libsdl3", {system = true})

add_rules("mode.debug", "mode.release")

target("game")
   set_kind("binary")
   add_files("src/*.c")
   add_includedirs("include")
   
   add_packages("libsdl3")
   
   if is_plat("linux") then
      add_cflags("-Wall", "-Wextra", {tools = "clang"})

      if is_mode("debug") then
         add_cflags("-g", "-fsanitize=address,undefined", {tools = "clang"})
         add_ldflags("-fsanitize=address,undefined", {tools = "clang"})
         add_cflags("-O0", {tools = "clang"})
         set_policy("build.sanitizer.address", true)
         set_policy("build.sanitizer.undefined", true)
      end
   end
