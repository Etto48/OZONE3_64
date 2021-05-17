CXXC		:=	g++
AS			:=	nasm
BIN_DIR		:=	bin
OBJ_DIR		:=	obj
SRC_DIR		:=	src
ISO_DIR		:=	iso

#BOOTSTRAP	:=	bootstrap.asm
BOOTSTRAP_FILE:= src/bootstrap.asm
BOOTSTRAP_OBJ:= obj/$(notdir $(BOOTSTRAP_FILE)).o
ASFILES		:=	$(wildcard $(SRC_DIR)/*.s)
CXXFILES	:=	$(wildcard $(SRC_DIR)/*.cpp)
OBJFILES	:=	$(addprefix $(OBJ_DIR)/,$(patsubst %.s,%.s.o,$(notdir $(ASFILES))) $(patsubst %.cpp,%.cpp.o,$(notdir $(CXXFILES)))) $(BOOTSTRAP_OBJ)
linker		:=	$(SRC_DIR)/linker.ld

CXX_ARGS	:=	-g -O3

SHARED_FLAGS = -fno-builtin -O2 -nostdinc -nostdlib -ffreestanding -g -Wall -Wextra \
               -Werror -I. -MMD -mno-red-zone

CFLAGS = -ffreestanding -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -nostdlib -lgcc
ASFLAGS = -f elf64

version		:=	3.0.0
SO_NAME		:=	ozone-$(version)
SO			:=	$(BIN_DIR)/$(SO_NAME).bin

.PHONY: clean test

all: $(SO)
test: clean $(SO)
	@echo Starting Emulation
	@qemu-system-x86_64 -kernel $(SO) -machine type=pc-i440fx-3.1

$(SO): $(linker) $(OBJFILES)
	@echo Creating $@
	@$(CXXC) -T $(linker) -o $@ $(OBJFILES) -z max-page-size=0x1000 $(CFLAGS) -no-pie

$(OBJ_DIR)/%.s.o: $(SRC_DIR)/%.s
	@echo Creating $@
	@$(CXXC) -c $(CFLAGS) $< -o $@ 

$(OBJ_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	@echo Creating $@
	@$(CXXC) -c $(CFLAGS) $< -o $@ 

$(OBJ_DIR)/bootstrap.asm.o: $(SRC_DIR)/bootstrap.asm
	@echo Creating $@
	@$(AS) -felf64 $< -o $@

clean:
	@echo Cleaning Object Files
	@-rm $(OBJ_DIR)/*