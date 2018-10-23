# supported targets:
#   all, <default>: update dependencies on .h files, then build game
#   run: build strict, then execute with stderr redirected to errors FIFO
#   console: run error console printing contents of errors FIFO
#   game: just build game without updating dependencies
#   depend: explicitly rebuild dependencies
#   ansic: test compile as ANSI C
#   clean: clean up
#   wtf: print out some implicit rules used by this make file
#   strict: build game, disallowing warnings
#   debug: build game for debugging

DEPDIR := .d
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
BUILDDIR := build

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@


# define variable with list of libraries, name of variable is defined by make itself, it uses this in the default rule for linking
LDLIBS = -lcurses -lm

# first target in the file is automatic default, "all" is a traditional name
# this target has no steps, it is just used to make it dependent on all the real targets so they are all built (only one right now)
all: game

# this project contains multiple C files, which are dependent on header files that we find using makedepend, so header files  are not listed here
SRCS := $(shell find -name "*.c" -not -path "./tests/*" -not -path "./game.c")
OBJS := $(patsubst %.c,$(BUILDDIR)/%.o,$(SRCS))
$(shell mkdir -p `dirname $(SRCS) | sed -e 's/^/$(DEPDIR)\//'`)
$(shell mkdir -p `dirname $(SRCS) | sed -e 's/^/$(BUILDDIR)\//'`)


print-%  : ; @echo $* = $($*)



$(BUILDDIR)/%.o : %.c
$(BUILDDIR)/%.o : %.c $(DEPDIR)/%*.d
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

# this target is dependent on all objects
# the rules for building are taken from the implicit rule from multiple .o files to a file with no extension 
# (link object files and libs, which is where LDLIBS variable is used)
# NOTE that "game" is actually a binary created, so this won't run again if the file "game" is newer than all the .o files
game: $(OBJS)

# in order to build the object files, make will search for implicit rules how to build and it will find the rule from .c to .o (compile to object code)
# so we don't need to specify any targets to build those

# NOTE that other targets like all, clean, ansic do not correspond to actual files created, so they always run if called, because their
# target file is not there
strict:
	$(MAKE) CFLAGS="-Werror" game

debug:
	$(MAKE) clean
	$(MAKE) CFLAGS="-g" LDFLAGS="-g" game
	gdb ./game 2> errors || reset

# clean up stuff, one step (note steps are tab-indented lines, each of which is executed as shell command in a subprocess using $(SHELL)
# as the executable)
clean:
	rm -f game ansic $(OBJS)
	rm -fr $(DEPDIR)

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
console: errors
	tail -f errors 

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))
