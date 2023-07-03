# Setup console for windows...
ifeq ($(OS),Windows_NT)

	SHELL := powershell.exe
	.SHELLFLAGS := -Command
	BOLD   = 1
	BLACK  = Black
	RED    = Red
	GREEN  = Green
	YELLOW = Yellow
	BLUE   = Blue
	PURPLE = Magenta
	CYAN   = Cyan
	WHITE  = White

else

	BOLD   = 0
	BLACK  = \e[$(BOLD);90m
	RED    = \e[$(BOLD);31m
	GREEN  = \e[$(BOLD);92m
	YELLOW = \e[$(BOLD);93m
	BLUE   = \e[$(BOLD);94m
	PURPLE = \e[$(BOLD);35m
	CYAN   = \e[$(BOLD);96m
	WHITE  = \e[$(BOLD);97m

endif

#Static library build...
STATIC= asigraph
# Debug mode includes viwerr.h and -DAG_DEV...
DEBUG  = t
# Debug/release flags...
DFLAGS = 
# Viwerr library home directory...
VIWERR = ext/viwerr/viwerr
# Test path...
EXECUTE_TEST = ./test/a
# Compiler...
CC = gcc
# C standard...
STD = gnu2x
# Compiler flags...
CFLAGS = -Wall -Wextra -Wpedantic \
	 -Wformat=2 -Wno-unused-parameter -Wshadow \
	 -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
	 -Wredundant-decls -Wnested-externs -Wmissing-include-dirs \
	 -pipe -std=$(STD)
CFLAGSTEST = 


# Printing setup
PRINT_COLOR =
PRINT =
ifeq ($(OS), Windows_NT)
	PRINT =Write-Host
	PRINT_COLOR = -ForegroundColor $(1) "$(2)"
else
	PRINT =echo
	PRINT_COLOR = -e "$(1)$(2)\e[0;97m"
endif

# Setup debug commands and libraries to work on OS basis...
ifeq ($(DEBUG), f) 
	undefine VIWERR
	DFLAGS = -O3
else
	ifeq ($(OS),Windows_NT)
		VIWERR := $(addsuffix .lib, $(VIWERR))
	else
		VIWERR := $(addsuffix .a, $(VIWERR))
	endif
	DFLAGS = -g -DAG_DEV
endif

# List all source directories and create a list of their files for object
# compilation...
SOURCE_DIRS = ./src
SRC = $(foreach D,$(SOURCE_DIRS),$(wildcard $(D)/*.c))   
OBJ = $(patsubst %.c,%.o,$(SRC))   

# Setup static suffix, executable suffix and cleanup functionality
# on OS basis...
REMOVE =
ifeq ($(OS),Windows_NT)
    EXECUTE_TEST := $(addsuffix .exe, $(EXECUTE_TEST))
    STATIC := $(addsuffix .lib, $(STATIC))
    REMOVE = Get-ChildItem * -Include *.o, *.lib, *.exe -Recurse | Remove-Item
else
    EXECUTE_TEST := $(addsuffix .out, $(EXECUTE_TEST))
    STATIC := $(addsuffix .a, $(STATIC))
    REMOVE = rm -f $(OBJ) $(STATIC) $(EXECUTE_TEST)
endif

# Compile static library...
$(STATIC): $(OBJ) $(VIWERR)
	@$(PRINT) $(call PRINT_COLOR,$(PURPLE),[Link (Static)])
	ar rcs $@ $(OBJ)

# Compile objects...
%.o: %.c $(VIWERR)
	@$(PRINT) $(call PRINT_COLOR,$(GREEN),- Compiling:) $<
	@$(CC) -lm -c $(DFLAGS) $(CFLAGS) $< -o $@

# Compile external error detection library...
$(VIWERR):
	@$(PRINT) $(call PRINT_COLOR,$(PURPLE),"---:VIWERR:---")
	make -C ext/viwerr

# Cleanup...
.PHONY: clean
clean: 
	@$(PRINT) $(call PRINT_COLOR,$(RED),[Deleting all objects libraries and executables])
	$(REMOVE)

# Create static library, and run test...
.PHONY: test
test: $(STATIC)
	@$(PRINT) $(call PRINT_COLOR,$(PURPLE),[Running test/test.c])
	@$(CC) -lm -g test/test.c -o $(EXECUTE_TEST) $(STATIC) $(VIWERR) -std=$(STD) $(CFLAGSTEST)
	@$(EXECUTE_TEST)

# Clean then test...
.PHONY: rebuild
rebuild: clean test