add_rules("mode.debug", "mode.release")

target("generador_mapas")
    set_kind("binary")
    
    set_languages("c99")
    
    add_defines("kiss_fft_scalar=float")
    
    add_cxflags("-Wall", "-Wextra")
    
    add_links("m")
    
    add_files("*.c")
