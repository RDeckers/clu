rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *, %,$2),$d))
directories=$(sort $(dir $(wildcard $/*)))
ifeq ($(OS),Windows_NT)
FIX_PATH = $(subst /,\,$1)
	REPORT = @echo $1
	CHK_DIR_EXISTS = if not exist "$(strip $1)" mkdir "$(strip $1)"
	NUKE = rmdir /s /q
	COPY_DIR = xcopy $(call FIX_PATH,$1 $2) /E /H /Y
	COPY_CONTENT = xcopy /s /Y $(call FIX_PATH,$1 $2)
	COPY = xcopy $(call FIX_PATH,$1 $2) /Y
	INSTALL_LIB_DIR := Z:/lib/
	INSTALL_BIN_DIR := Z:/bin/
	INSTALL_INCLUDE_DIR := Z:/include/
	LIB_SUFFIX =.dll
	OPENCL_HEADERS :=$(call rwildcard, Z:/include/CL/, *.h)
else
	REPORT = @echo -e "\e[4;1;37m$1\033[0m"
	CHK_DIR_EXISTS = test -d $1 || mkdir -p $1
	NUKE = rm -r $1
	COPY_DIR = cp -rv $1 $2
	FIX_PATH = $1
	INSTALL_LIB_DIR := ~/lib/
	INSTALL_BIN_DIR := ~/bin/
	INSTALL_INCLUDE_DIR := ~/include/
	LIB_SUFFIX :=.so
	OPENCL_HEADERS :=/usr/include/CL/*.h
endif



PROJECT_DIR :=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))
OBJ_DIR := $(PROJECT_DIR)obj
BIN_DIR := $(PROJECT_DIR)bin
EXAMPLE_BIN_DIR := $(BIN_DIR)/examples
SRC_DIR := $(PROJECT_DIR)src
LIB_DIR := $(PROJECT_DIR)lib
EXAMPLE_DIR := $(PROJECT_DIR)/examples
C_FILES := $(call rwildcard, $(SRC_DIR), *.c)
AWK_FILES := $(call rwildcard, $(SRC_DIR), *.awk)
CFIED_AWK_FILES := $(subst .awk,.c, $(AWK_FILES))
C_FILES += $(CFIED_AWK_FILES)
EXAMPLE_C_FILES := $(call rwildcard, $(EXAMPLE_DIR), *.c)
EXAMPLES := $(patsubst $(EXAMPLE_DIR)/%.c, $(EXAMPLE_BIN_DIR)/%, $(EXAMPLE_C_FILES))

OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_FILES))

LIB_NAME := clu

LD_FLAGS += --std=gnu99 -march=native -lOpenCL -lutilities -L$(INSTALL_LIB_DIR)
C_FLAGS += --std=gnu99 -O2 -pipe -march=native -I$(PROJECT_DIR)headers -I$(INSTALL_INCLUDE_DIR)


lib: $(OBJ_FILES)
	$(call REPORT,Building $@)
	$(call CHK_DIR_EXISTS, $(LIB_DIR))
	gcc -shared -o $(LIB_DIR)/lib$(LIB_NAME)$(LIB_SUFFIX) $(OBJ_FILES) $(LD_FLAGS)

install: lib
	$(call REPORT, Installing files...)
	$(call COPY, $(LIB_DIR)/lib$(LIB_NAME)$(LIB_SUFFIX),$(INSTALL_LIB_DIR))
	$(call COPY_CONTENT, $(PROJECT_DIR)headers,$(INSTALL_INCLUDE_DIR))

all : lib examples

examples: $(OBJ_FILES) $(EXAMPLES)

$(EXAMPLE_BIN_DIR)% : $(EXAMPLE_DIR)%.c
	$(call REPORT,Building $@)
	$(call CHK_DIR_EXISTS, $(dir $@))
	gcc $(C_FLAGS) -o "$@" "$<" $(OBJ_FILES) $(LD_FLAGS)

$(SRC_DIR)/%.c: $(SRC_DIR)/%.awk
	$(call REPORT,Generating $@)
	awk -f "$<" $(OPENCL_HEADERS) >> "$@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(call REPORT,Compiling $@)
	$(call CHK_DIR_EXISTS, $(dir $@))
	@gcc -fpic $(C_FLAGS) -o "$@" -c "$<"

clean:
	$(call REPORT,Cleaning...)
	-$(NUKE) "$(EXAMPLE_BIN_DIR)" "$(OBJ_DIR)" "$(BIN_DIR)" "$(LIB_DIR)"
