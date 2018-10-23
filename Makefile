# supported targets:
#   all, <default>: update dependencies on .h files, then build game
#   run: build strict, then execute with stderr redirected to .errors FIFO
#   console: run error console printing contents of .errors FIFO
#   game: just build game without updating dependencies
#   depend: explicitly rebuild dependencies
#   ansic: test compile as ANSI C
#   clean: clean up
#   wtf: print out some implicit rules used by this make file
#   strict: build game, disallowing warnings
#   debug: build game for debugging

# define variable with list of libraries, name of variable is defined by make itself, it uses this in the default rule for linking
LDLIBS = -lcurses -lm

# first target in the file is automatic default, "all" is a traditional name
# this target has no steps, it is just used to make it dependent on all the real targets so they are all built (only one right now)
all: depend game

# this project contains multiple C files, which are dependent on header files that we find using makedepend, so header files  are not listed here
SRCS  = game.c level/level.c mob/mob.c chemistry/chemistry.c color/color.c los/los.c simulation/vector.c simulation/min_heap.c simulation/simulation.c log.c
OBJS := $(patsubst %.c,%.o,$(SRCS))

# this target is dependent on all objects
# the rules for building are taken from the implicit rule from multiple .o files to a file with no extension 
# (link object files and libs, which is where LDLIBS variable is used)
# NOTE that "game" is actually a binary created, so this won't run again if the file "game" is newer than all the .o files
game: $(OBJS)

# in order to build the object files, make will search for implicit rules how to build and it will find the rule from .c to .o (compile to object code)
# so we don't need to specify any targets to build those

# NOTE that other targets like all, depend, clean, ansic do not correspond to actual files created, so they always run if called, because their
# target file is not there
depend:
	makedepend -- -I/usr/local/include -- $(SRCS) > /dev/null 2>&1
	@sed -e '1,/^# DO NOT DELETE/d' < Makefile
strict:
	$(MAKE) CFLAGS="-Werror" game

debug:
	$(MAKE) clean
	$(MAKE) CFLAGS="-g" LDFLAGS="-g" game
	gdb ./game 2> .errors || reset

# clean up stuff, one step (note steps are tab-indented lines, each of which is executed as shell command in a subprocess using $(SHELL)
# as the executable)
clean:
	rm -f game ansic $(OBJS)

# target for ANSI C compilation, forks another copy of make, running with the additional variable CFLAGS set to options to use for all compiles
# this sub-make builds the target "all" explicitly just to make sure we don't create infinite loop by making "ansic" by mistake
ansic:
	$(MAKE) CFLAGS="-ansi -pedantic" all

c18:
	$(MAKE) CFLAGS="-std=c18" all


test_suite: chemistry/chemistry.c tests/chemistry/check_chemistry.c simulation/min_heap.c tests/simulation/check_min_heap.c tests/check_check.c tests/simulation/check_simulation.c simulation/simulation.c simulation/vector.c tests/simulation/check_vector.c
	$(CC) $^ -lcheck -lm -g -Wall -o $@

# print out some implicit rules used in this file so you can see how variables are used by implicit rules
wtf:
	$(MAKE) --silent -p -f /dev/null | egrep -A 5 \
		-e '^.c.o:' \
		-e '^.c:' \
		-e 'COMPILE.c =' \
		-e 'LINK.c ='

.errors:
	mkfifo .errors
run: strict .errors
	ENABLE_LOG=1 ./game 2> .errors || reset
# This only checks that "errors" exists, not whether it's a pipe
console: .errors
	tail -f .errors 

# DO NOT DELETE

game.o: /usr/include/time.h /usr/include/features.h
game.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
game.o: /usr/include/bits/wordsize.h /usr/include/bits/long-double.h
game.o: /usr/include/gnu/stubs.h /usr/include/bits/time.h
game.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
game.o: /usr/include/bits/types/clock_t.h /usr/include/bits/types/time_t.h
game.o: /usr/include/bits/types/struct_tm.h
game.o: /usr/include/bits/types/struct_timespec.h
game.o: /usr/include/bits/types/clockid_t.h /usr/include/bits/types/timer_t.h
game.o: /usr/include/bits/types/struct_itimerspec.h
game.o: /usr/include/bits/types/locale_t.h
game.o: /usr/include/bits/types/__locale_t.h /usr/include/stdlib.h
game.o: /usr/include/bits/libc-header-start.h /usr/include/bits/waitflags.h
game.o: /usr/include/bits/waitstatus.h /usr/include/bits/floatn.h
game.o: /usr/include/bits/floatn-common.h /usr/include/sys/types.h
game.o: /usr/include/bits/stdint-intn.h /usr/include/endian.h
game.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
game.o: /usr/include/bits/uintn-identity.h /usr/include/sys/select.h
game.o: /usr/include/bits/select.h /usr/include/bits/types/sigset_t.h
game.o: /usr/include/bits/types/__sigset_t.h
game.o: /usr/include/bits/types/struct_timeval.h
game.o: /usr/include/bits/pthreadtypes.h
game.o: /usr/include/bits/thread-shared-types.h
game.o: /usr/include/bits/pthreadtypes-arch.h /usr/include/alloca.h
game.o: /usr/include/bits/stdlib-float.h /usr/include/ncurses.h
game.o: /usr/include/ncurses_dll.h /usr/include/stdint.h
game.o: /usr/include/bits/wchar.h /usr/include/bits/stdint-uintn.h
game.o: /usr/include/stdio.h /usr/include/bits/types/__fpos_t.h
game.o: /usr/include/bits/types/__mbstate_t.h
game.o: /usr/include/bits/types/__fpos64_t.h /usr/include/bits/types/__FILE.h
game.o: /usr/include/bits/types/FILE.h /usr/include/bits/types/struct_FILE.h
game.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
game.o: /usr/include/unctrl.h /usr/include/curses.h /usr/include/string.h
game.o: /usr/include/strings.h game.h game_cfg/game_cfg.h game_cfg/colors.h
game.o: game_cfg/game_internals.h game_cfg/setup.h game_cfg/symbols.h log.h
game.o: level/level.h game.h mob/mob.h game_cfg/game_cfg.h
game.o: simulation/simulation.h simulation/min_heap.h simulation/vector.h
game.o: simulation/vector.h chemistry/chemistry.h los/los.h
game.o: /usr/include/math.h /usr/include/bits/math-vector.h
game.o: /usr/include/bits/libm-simd-decl-stubs.h
game.o: /usr/include/bits/flt-eval-method.h /usr/include/bits/fp-logb.h
game.o: /usr/include/bits/fp-fast.h
game.o: /usr/include/bits/mathcalls-helper-functions.h
game.o: /usr/include/bits/mathcalls.h /usr/include/bits/mathcalls-narrow.h
game.o: /usr/include/bits/iscanonical.h /usr/include/assert.h mob/mob.h
game.o: simulation/simulation.h los/los.h color/color.h game_cfg/colors.h
level/level.o: /usr/include/stdio.h /usr/include/bits/libc-header-start.h
level/level.o: /usr/include/features.h /usr/include/stdc-predef.h
level/level.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
level/level.o: /usr/include/bits/long-double.h /usr/include/gnu/stubs.h
level/level.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
level/level.o: /usr/include/bits/types/__fpos_t.h
level/level.o: /usr/include/bits/types/__mbstate_t.h
level/level.o: /usr/include/bits/types/__fpos64_t.h
level/level.o: /usr/include/bits/types/__FILE.h
level/level.o: /usr/include/bits/types/FILE.h
level/level.o: /usr/include/bits/types/struct_FILE.h
level/level.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
level/level.o: /usr/include/limits.h /usr/include/bits/posix1_lim.h
level/level.o: /usr/include/bits/local_lim.h /usr/include/linux/limits.h
level/level.o: /usr/include/bits/posix2_lim.h /usr/include/string.h
level/level.o: /usr/include/bits/types/locale_t.h
level/level.o: /usr/include/bits/types/__locale_t.h /usr/include/strings.h
level/level.o: /usr/include/math.h /usr/include/bits/math-vector.h
level/level.o: /usr/include/bits/libm-simd-decl-stubs.h
level/level.o: /usr/include/bits/floatn.h /usr/include/bits/floatn-common.h
level/level.o: /usr/include/bits/flt-eval-method.h
level/level.o: /usr/include/bits/fp-logb.h /usr/include/bits/fp-fast.h
level/level.o: /usr/include/bits/mathcalls-helper-functions.h
level/level.o: /usr/include/bits/mathcalls.h
level/level.o: /usr/include/bits/mathcalls-narrow.h
level/level.o: /usr/include/bits/iscanonical.h level/level.h
level/level.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
level/level.o: /usr/include/bits/waitstatus.h /usr/include/sys/types.h
level/level.o: /usr/include/bits/types/clock_t.h
level/level.o: /usr/include/bits/types/clockid_t.h
level/level.o: /usr/include/bits/types/time_t.h
level/level.o: /usr/include/bits/types/timer_t.h
level/level.o: /usr/include/bits/stdint-intn.h /usr/include/endian.h
level/level.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
level/level.o: /usr/include/bits/uintn-identity.h /usr/include/sys/select.h
level/level.o: /usr/include/bits/select.h /usr/include/bits/types/sigset_t.h
level/level.o: /usr/include/bits/types/__sigset_t.h
level/level.o: /usr/include/bits/types/struct_timeval.h
level/level.o: /usr/include/bits/types/struct_timespec.h
level/level.o: /usr/include/bits/pthreadtypes.h
level/level.o: /usr/include/bits/thread-shared-types.h
level/level.o: /usr/include/bits/pthreadtypes-arch.h /usr/include/alloca.h
level/level.o: /usr/include/bits/stdlib-float.h game.h mob/mob.h
level/level.o: game_cfg/game_cfg.h simulation/simulation.h
level/level.o: simulation/min_heap.h simulation/vector.h simulation/vector.h
level/level.o: chemistry/chemistry.h los/los.h /usr/include/assert.h log.h
mob/mob.o: /usr/include/limits.h /usr/include/bits/libc-header-start.h
mob/mob.o: /usr/include/features.h /usr/include/stdc-predef.h
mob/mob.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
mob/mob.o: /usr/include/bits/long-double.h /usr/include/gnu/stubs.h
mob/mob.o: /usr/include/bits/posix1_lim.h /usr/include/bits/local_lim.h
mob/mob.o: /usr/include/linux/limits.h /usr/include/bits/posix2_lim.h
mob/mob.o: /usr/include/math.h /usr/include/bits/types.h
mob/mob.o: /usr/include/bits/typesizes.h /usr/include/bits/math-vector.h
mob/mob.o: /usr/include/bits/libm-simd-decl-stubs.h
mob/mob.o: /usr/include/bits/floatn.h /usr/include/bits/floatn-common.h
mob/mob.o: /usr/include/bits/flt-eval-method.h /usr/include/bits/fp-logb.h
mob/mob.o: /usr/include/bits/fp-fast.h
mob/mob.o: /usr/include/bits/mathcalls-helper-functions.h
mob/mob.o: /usr/include/bits/mathcalls.h /usr/include/bits/mathcalls-narrow.h
mob/mob.o: /usr/include/bits/iscanonical.h /usr/include/stdio.h
mob/mob.o: /usr/include/bits/types/__fpos_t.h
mob/mob.o: /usr/include/bits/types/__mbstate_t.h
mob/mob.o: /usr/include/bits/types/__fpos64_t.h
mob/mob.o: /usr/include/bits/types/__FILE.h /usr/include/bits/types/FILE.h
mob/mob.o: /usr/include/bits/types/struct_FILE.h
mob/mob.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
mob/mob.o: game.h mob/mob.h /usr/include/stdlib.h
mob/mob.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
mob/mob.o: /usr/include/sys/types.h /usr/include/bits/types/clock_t.h
mob/mob.o: /usr/include/bits/types/clockid_t.h
mob/mob.o: /usr/include/bits/types/time_t.h /usr/include/bits/types/timer_t.h
mob/mob.o: /usr/include/bits/stdint-intn.h /usr/include/endian.h
mob/mob.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
mob/mob.o: /usr/include/bits/uintn-identity.h /usr/include/sys/select.h
mob/mob.o: /usr/include/bits/select.h /usr/include/bits/types/sigset_t.h
mob/mob.o: /usr/include/bits/types/__sigset_t.h
mob/mob.o: /usr/include/bits/types/struct_timeval.h
mob/mob.o: /usr/include/bits/types/struct_timespec.h
mob/mob.o: /usr/include/bits/pthreadtypes.h
mob/mob.o: /usr/include/bits/thread-shared-types.h
mob/mob.o: /usr/include/bits/pthreadtypes-arch.h /usr/include/alloca.h
mob/mob.o: /usr/include/bits/stdlib-float.h game_cfg/game_cfg.h
mob/mob.o: simulation/simulation.h simulation/min_heap.h simulation/vector.h
mob/mob.o: /usr/include/string.h /usr/include/bits/types/locale_t.h
mob/mob.o: /usr/include/bits/types/__locale_t.h /usr/include/strings.h
mob/mob.o: simulation/vector.h chemistry/chemistry.h level/level.h mob/mob.h
mob/mob.o: los/los.h /usr/include/assert.h
chemistry/chemistry.o: /usr/include/math.h
chemistry/chemistry.o: /usr/include/bits/libc-header-start.h
chemistry/chemistry.o: /usr/include/features.h /usr/include/stdc-predef.h
chemistry/chemistry.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
chemistry/chemistry.o: /usr/include/bits/long-double.h
chemistry/chemistry.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
chemistry/chemistry.o: /usr/include/bits/typesizes.h
chemistry/chemistry.o: /usr/include/bits/math-vector.h
chemistry/chemistry.o: /usr/include/bits/libm-simd-decl-stubs.h
chemistry/chemistry.o: /usr/include/bits/floatn.h
chemistry/chemistry.o: /usr/include/bits/floatn-common.h
chemistry/chemistry.o: /usr/include/bits/flt-eval-method.h
chemistry/chemistry.o: /usr/include/bits/fp-logb.h
chemistry/chemistry.o: /usr/include/bits/fp-fast.h
chemistry/chemistry.o: /usr/include/bits/mathcalls-helper-functions.h
chemistry/chemistry.o: /usr/include/bits/mathcalls.h
chemistry/chemistry.o: /usr/include/bits/mathcalls-narrow.h
chemistry/chemistry.o: /usr/include/bits/iscanonical.h /usr/include/string.h
chemistry/chemistry.o: /usr/include/bits/types/locale_t.h
chemistry/chemistry.o: /usr/include/bits/types/__locale_t.h
chemistry/chemistry.o: /usr/include/strings.h /usr/include/stdlib.h
chemistry/chemistry.o: /usr/include/bits/waitflags.h
chemistry/chemistry.o: /usr/include/bits/waitstatus.h
chemistry/chemistry.o: /usr/include/sys/types.h
chemistry/chemistry.o: /usr/include/bits/types/clock_t.h
chemistry/chemistry.o: /usr/include/bits/types/clockid_t.h
chemistry/chemistry.o: /usr/include/bits/types/time_t.h
chemistry/chemistry.o: /usr/include/bits/types/timer_t.h
chemistry/chemistry.o: /usr/include/bits/stdint-intn.h /usr/include/endian.h
chemistry/chemistry.o: /usr/include/bits/endian.h
chemistry/chemistry.o: /usr/include/bits/byteswap.h
chemistry/chemistry.o: /usr/include/bits/uintn-identity.h
chemistry/chemistry.o: /usr/include/sys/select.h /usr/include/bits/select.h
chemistry/chemistry.o: /usr/include/bits/types/sigset_t.h
chemistry/chemistry.o: /usr/include/bits/types/__sigset_t.h
chemistry/chemistry.o: /usr/include/bits/types/struct_timeval.h
chemistry/chemistry.o: /usr/include/bits/types/struct_timespec.h
chemistry/chemistry.o: /usr/include/bits/pthreadtypes.h
chemistry/chemistry.o: /usr/include/bits/thread-shared-types.h
chemistry/chemistry.o: /usr/include/bits/pthreadtypes-arch.h
chemistry/chemistry.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h
chemistry/chemistry.o: chemistry/chemistry.h
color/color.o: /usr/include/ncurses.h /usr/include/ncurses_dll.h
color/color.o: /usr/include/stdint.h /usr/include/bits/libc-header-start.h
color/color.o: /usr/include/features.h /usr/include/stdc-predef.h
color/color.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
color/color.o: /usr/include/bits/long-double.h /usr/include/gnu/stubs.h
color/color.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
color/color.o: /usr/include/bits/wchar.h /usr/include/bits/stdint-intn.h
color/color.o: /usr/include/bits/stdint-uintn.h /usr/include/stdio.h
color/color.o: /usr/include/bits/types/__fpos_t.h
color/color.o: /usr/include/bits/types/__mbstate_t.h
color/color.o: /usr/include/bits/types/__fpos64_t.h
color/color.o: /usr/include/bits/types/__FILE.h
color/color.o: /usr/include/bits/types/FILE.h
color/color.o: /usr/include/bits/types/struct_FILE.h
color/color.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
color/color.o: /usr/include/unctrl.h /usr/include/curses.h color/color.h
color/color.o: game_cfg/colors.h
los/los.o: los/los.h /usr/include/math.h
los/los.o: /usr/include/bits/libc-header-start.h /usr/include/features.h
los/los.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
los/los.o: /usr/include/bits/wordsize.h /usr/include/bits/long-double.h
los/los.o: /usr/include/gnu/stubs.h /usr/include/bits/types.h
los/los.o: /usr/include/bits/typesizes.h /usr/include/bits/math-vector.h
los/los.o: /usr/include/bits/libm-simd-decl-stubs.h
los/los.o: /usr/include/bits/floatn.h /usr/include/bits/floatn-common.h
los/los.o: /usr/include/bits/flt-eval-method.h /usr/include/bits/fp-logb.h
los/los.o: /usr/include/bits/fp-fast.h
los/los.o: /usr/include/bits/mathcalls-helper-functions.h
los/los.o: /usr/include/bits/mathcalls.h /usr/include/bits/mathcalls-narrow.h
los/los.o: /usr/include/bits/iscanonical.h /usr/include/assert.h
los/los.o: /usr/include/stdlib.h /usr/include/bits/waitflags.h
los/los.o: /usr/include/bits/waitstatus.h /usr/include/sys/types.h
los/los.o: /usr/include/bits/types/clock_t.h
los/los.o: /usr/include/bits/types/clockid_t.h
los/los.o: /usr/include/bits/types/time_t.h /usr/include/bits/types/timer_t.h
los/los.o: /usr/include/bits/stdint-intn.h /usr/include/endian.h
los/los.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
los/los.o: /usr/include/bits/uintn-identity.h /usr/include/sys/select.h
los/los.o: /usr/include/bits/select.h /usr/include/bits/types/sigset_t.h
los/los.o: /usr/include/bits/types/__sigset_t.h
los/los.o: /usr/include/bits/types/struct_timeval.h
los/los.o: /usr/include/bits/types/struct_timespec.h
los/los.o: /usr/include/bits/pthreadtypes.h
los/los.o: /usr/include/bits/thread-shared-types.h
los/los.o: /usr/include/bits/pthreadtypes-arch.h /usr/include/alloca.h
los/los.o: /usr/include/bits/stdlib-float.h /usr/include/stdio.h
los/los.o: /usr/include/bits/types/__fpos_t.h
los/los.o: /usr/include/bits/types/__mbstate_t.h
los/los.o: /usr/include/bits/types/__fpos64_t.h
los/los.o: /usr/include/bits/types/__FILE.h /usr/include/bits/types/FILE.h
los/los.o: /usr/include/bits/types/struct_FILE.h
los/los.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
simulation/vector.o: simulation/vector.h /usr/include/stdlib.h
simulation/vector.o: /usr/include/bits/libc-header-start.h
simulation/vector.o: /usr/include/features.h /usr/include/stdc-predef.h
simulation/vector.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
simulation/vector.o: /usr/include/bits/long-double.h /usr/include/gnu/stubs.h
simulation/vector.o: /usr/include/bits/waitflags.h
simulation/vector.o: /usr/include/bits/waitstatus.h
simulation/vector.o: /usr/include/bits/floatn.h
simulation/vector.o: /usr/include/bits/floatn-common.h
simulation/vector.o: /usr/include/sys/types.h /usr/include/bits/types.h
simulation/vector.o: /usr/include/bits/typesizes.h
simulation/vector.o: /usr/include/bits/types/clock_t.h
simulation/vector.o: /usr/include/bits/types/clockid_t.h
simulation/vector.o: /usr/include/bits/types/time_t.h
simulation/vector.o: /usr/include/bits/types/timer_t.h
simulation/vector.o: /usr/include/bits/stdint-intn.h /usr/include/endian.h
simulation/vector.o: /usr/include/bits/endian.h /usr/include/bits/byteswap.h
simulation/vector.o: /usr/include/bits/uintn-identity.h
simulation/vector.o: /usr/include/sys/select.h /usr/include/bits/select.h
simulation/vector.o: /usr/include/bits/types/sigset_t.h
simulation/vector.o: /usr/include/bits/types/__sigset_t.h
simulation/vector.o: /usr/include/bits/types/struct_timeval.h
simulation/vector.o: /usr/include/bits/types/struct_timespec.h
simulation/vector.o: /usr/include/bits/pthreadtypes.h
simulation/vector.o: /usr/include/bits/thread-shared-types.h
simulation/vector.o: /usr/include/bits/pthreadtypes-arch.h
simulation/vector.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h
simulation/vector.o: /usr/include/string.h /usr/include/bits/types/locale_t.h
simulation/vector.o: /usr/include/bits/types/__locale_t.h
simulation/vector.o: /usr/include/strings.h /usr/include/stdio.h
simulation/vector.o: /usr/include/bits/types/__fpos_t.h
simulation/vector.o: /usr/include/bits/types/__mbstate_t.h
simulation/vector.o: /usr/include/bits/types/__fpos64_t.h
simulation/vector.o: /usr/include/bits/types/__FILE.h
simulation/vector.o: /usr/include/bits/types/FILE.h
simulation/vector.o: /usr/include/bits/types/struct_FILE.h
simulation/vector.o: /usr/include/bits/stdio_lim.h
simulation/vector.o: /usr/include/bits/sys_errlist.h
simulation/min_heap.o: /usr/include/stdlib.h
simulation/min_heap.o: /usr/include/bits/libc-header-start.h
simulation/min_heap.o: /usr/include/features.h /usr/include/stdc-predef.h
simulation/min_heap.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
simulation/min_heap.o: /usr/include/bits/long-double.h
simulation/min_heap.o: /usr/include/gnu/stubs.h /usr/include/bits/waitflags.h
simulation/min_heap.o: /usr/include/bits/waitstatus.h
simulation/min_heap.o: /usr/include/bits/floatn.h
simulation/min_heap.o: /usr/include/bits/floatn-common.h
simulation/min_heap.o: /usr/include/sys/types.h /usr/include/bits/types.h
simulation/min_heap.o: /usr/include/bits/typesizes.h
simulation/min_heap.o: /usr/include/bits/types/clock_t.h
simulation/min_heap.o: /usr/include/bits/types/clockid_t.h
simulation/min_heap.o: /usr/include/bits/types/time_t.h
simulation/min_heap.o: /usr/include/bits/types/timer_t.h
simulation/min_heap.o: /usr/include/bits/stdint-intn.h /usr/include/endian.h
simulation/min_heap.o: /usr/include/bits/endian.h
simulation/min_heap.o: /usr/include/bits/byteswap.h
simulation/min_heap.o: /usr/include/bits/uintn-identity.h
simulation/min_heap.o: /usr/include/sys/select.h /usr/include/bits/select.h
simulation/min_heap.o: /usr/include/bits/types/sigset_t.h
simulation/min_heap.o: /usr/include/bits/types/__sigset_t.h
simulation/min_heap.o: /usr/include/bits/types/struct_timeval.h
simulation/min_heap.o: /usr/include/bits/types/struct_timespec.h
simulation/min_heap.o: /usr/include/bits/pthreadtypes.h
simulation/min_heap.o: /usr/include/bits/thread-shared-types.h
simulation/min_heap.o: /usr/include/bits/pthreadtypes-arch.h
simulation/min_heap.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h
simulation/min_heap.o: /usr/include/stdio.h
simulation/min_heap.o: /usr/include/bits/types/__fpos_t.h
simulation/min_heap.o: /usr/include/bits/types/__mbstate_t.h
simulation/min_heap.o: /usr/include/bits/types/__fpos64_t.h
simulation/min_heap.o: /usr/include/bits/types/__FILE.h
simulation/min_heap.o: /usr/include/bits/types/FILE.h
simulation/min_heap.o: /usr/include/bits/types/struct_FILE.h
simulation/min_heap.o: /usr/include/bits/stdio_lim.h
simulation/min_heap.o: /usr/include/bits/sys_errlist.h simulation/min_heap.h
simulation/min_heap.o: simulation/vector.h /usr/include/string.h
simulation/min_heap.o: /usr/include/bits/types/locale_t.h
simulation/min_heap.o: /usr/include/bits/types/__locale_t.h
simulation/min_heap.o: /usr/include/strings.h
simulation/simulation.o: /usr/include/time.h /usr/include/features.h
simulation/simulation.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
simulation/simulation.o: /usr/include/bits/wordsize.h
simulation/simulation.o: /usr/include/bits/long-double.h
simulation/simulation.o: /usr/include/gnu/stubs.h /usr/include/bits/time.h
simulation/simulation.o: /usr/include/bits/types.h
simulation/simulation.o: /usr/include/bits/typesizes.h
simulation/simulation.o: /usr/include/bits/types/clock_t.h
simulation/simulation.o: /usr/include/bits/types/time_t.h
simulation/simulation.o: /usr/include/bits/types/struct_tm.h
simulation/simulation.o: /usr/include/bits/types/struct_timespec.h
simulation/simulation.o: /usr/include/bits/types/clockid_t.h
simulation/simulation.o: /usr/include/bits/types/timer_t.h
simulation/simulation.o: /usr/include/bits/types/struct_itimerspec.h
simulation/simulation.o: /usr/include/bits/types/locale_t.h
simulation/simulation.o: /usr/include/bits/types/__locale_t.h
simulation/simulation.o: /usr/include/limits.h
simulation/simulation.o: /usr/include/bits/libc-header-start.h
simulation/simulation.o: /usr/include/bits/posix1_lim.h
simulation/simulation.o: /usr/include/bits/local_lim.h
simulation/simulation.o: /usr/include/linux/limits.h
simulation/simulation.o: /usr/include/bits/posix2_lim.h /usr/include/stdlib.h
simulation/simulation.o: /usr/include/bits/waitflags.h
simulation/simulation.o: /usr/include/bits/waitstatus.h
simulation/simulation.o: /usr/include/bits/floatn.h
simulation/simulation.o: /usr/include/bits/floatn-common.h
simulation/simulation.o: /usr/include/sys/types.h
simulation/simulation.o: /usr/include/bits/stdint-intn.h
simulation/simulation.o: /usr/include/endian.h /usr/include/bits/endian.h
simulation/simulation.o: /usr/include/bits/byteswap.h
simulation/simulation.o: /usr/include/bits/uintn-identity.h
simulation/simulation.o: /usr/include/sys/select.h /usr/include/bits/select.h
simulation/simulation.o: /usr/include/bits/types/sigset_t.h
simulation/simulation.o: /usr/include/bits/types/__sigset_t.h
simulation/simulation.o: /usr/include/bits/types/struct_timeval.h
simulation/simulation.o: /usr/include/bits/pthreadtypes.h
simulation/simulation.o: /usr/include/bits/thread-shared-types.h
simulation/simulation.o: /usr/include/bits/pthreadtypes-arch.h
simulation/simulation.o: /usr/include/alloca.h
simulation/simulation.o: /usr/include/bits/stdlib-float.h /usr/include/math.h
simulation/simulation.o: /usr/include/bits/math-vector.h
simulation/simulation.o: /usr/include/bits/libm-simd-decl-stubs.h
simulation/simulation.o: /usr/include/bits/flt-eval-method.h
simulation/simulation.o: /usr/include/bits/fp-logb.h
simulation/simulation.o: /usr/include/bits/fp-fast.h
simulation/simulation.o: /usr/include/bits/mathcalls-helper-functions.h
simulation/simulation.o: /usr/include/bits/mathcalls.h
simulation/simulation.o: /usr/include/bits/mathcalls-narrow.h
simulation/simulation.o: /usr/include/bits/iscanonical.h
simulation/simulation.o: simulation/simulation.h simulation/min_heap.h
simulation/simulation.o: simulation/vector.h /usr/include/string.h
simulation/simulation.o: /usr/include/strings.h
log.o: /usr/include/stdio.h /usr/include/bits/libc-header-start.h
log.o: /usr/include/features.h /usr/include/stdc-predef.h
log.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
log.o: /usr/include/bits/long-double.h /usr/include/gnu/stubs.h
log.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
log.o: /usr/include/bits/types/__fpos_t.h
log.o: /usr/include/bits/types/__mbstate_t.h
log.o: /usr/include/bits/types/__fpos64_t.h /usr/include/bits/types/__FILE.h
log.o: /usr/include/bits/types/FILE.h /usr/include/bits/types/struct_FILE.h
log.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h log.h
