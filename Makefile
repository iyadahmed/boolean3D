link_flags = -lm
compile_flags = -Wall -Wextra -Werror=return-type

release_build_subdir = Release
release_compile_flags = -O2 -flto
release_link_flags =

debug_build_subdir = Debug
debug_compile_flags = -g
debug_link_flags =

build_subdir = $(release_build_subdir)

bvh: main.c bvh.h read_stl.h statistics.h
	mkdir -p ./build/$(build_subdir)
	cc main.c -o ./build/$(build_subdir)/bvh $(link_flags) $(compile_flags)

debug: bvh
debug: build_subdir = $(debug_build_subdir)
debug: compile_flags += $(debug_compile_flags)
debug: link_flags += $(debug_link_flags)

clean:
	rm -r ./build