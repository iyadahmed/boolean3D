link_flags = -lm
compile_flags = -Wall -Wextra -Werror
build_subdir = Release

bvh: main.c bvh.h read_stl.h statistics.h
	mkdir -p ./build/$(build_subdir)
	gcc main.c -o ./build/$(build_subdir)/bvh $(link_flags) $(compile_flags)

debug: compile_flags += -g
debug: build_subdir = Debug
debug: bvh

clean:
	rm -r ./build