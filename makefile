# Setup console for windows...
ifeq ($(OS),Windows_NT)
	SHELL := powershell.exe
	.SHELLFLAGS := -Command
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
# Compiler flags...
CFLAGS = -Wall -Wextra -Wpedantic \
	 -Wformat=2 -Wno-unused-parameter -Wshadow \
	 -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
	 -Wredundant-decls -Wnested-externs -Wmissing-include-dirs \
	 -pipe -std=c2x

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

# Setup static suffix, executable suffix and cleanup functionality
# on OS basis...
ifeq ($(OS),Windows_NT)
    EXECUTE_TEST := $(addsuffix .exe, $(EXECUTE_TEST))
    STATIC := $(addsuffix .lib, $(STATIC))
    REMOVE = Get-ChildItem * -Include *.o, *.lib, *.exe -Recurse | Remove-Item
else
    EXECUTE_TEST := $(addsuffix .out, $(EXECUTE_TEST))
    STATIC := $(addsuffix .a, $(STATIC))
    REMOVE = rm -f $(OBJ) $(STATIC)
endif

# List all source directories and create a list of their files for object
# compilation...
SOURCE_DIRS = ./src
SRC = $(foreach D,$(SOURCES),$(wildcard $(D)/*.c))   
OBJ = $(patsubst %.c,%.o,$(SRC))   

# Compile static library...
$(STATIC): $(OBJ) $(VIWERR)
	@echo "[Link (Static)]"
	ar rcs $@ $(OBJ)

# Compile objects...
%.o: %.c $(VIWERR)
	@echo [Compile] $<
	@$(CC) -lm -c $(DFLAGS) $(CFLAGS) $< -o $@

# Compile external error detection library...
$(VIWERR):
	@echo "---:VIWERR:---"
	make -C ext/viwerr

# Cleanup...
.PHONY: clean
clean: 
	@$(REMOVE)

# Create static library, and run test...
.PHONY: test
test: $(STATIC)
	@echo "[Running test/test.c]"
	@$(CC) -lm -g test/test.c -o $(EXECUTE_TEST) $(STATIC) $(VIWERR)
	@$(EXECUTE_TEST)

# Clean then test...
.PHONY: rebuild
rebuild: clean test