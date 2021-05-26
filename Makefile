CXXC		:=	g++
AS			:=	as
BIN_DIR		:=	bin
OBJ_DIR		:=	obj
SRC_DIR		:=	src
ISO_DIR		:=	iso


#BOOTSTRAP_FILE:= src/bootstrap.asm
#BOOTSTRAP_OBJ:= obj/$(notdir $(BOOTSTRAP_FILE)).o
ASFILES		:=	$(wildcard $(SRC_DIR)/*.s)
CXXFILES	:=	$(wildcard $(SRC_DIR)/*.cpp)
OBJFILES	:=	$(addprefix $(OBJ_DIR)/,$(patsubst %.s,%.s.o,$(notdir $(ASFILES))) $(patsubst %.cpp,%.cpp.o,$(notdir $(CXXFILES)))) $(BOOTSTRAP_OBJ)
linker		:=	$(SRC_DIR)/linker.ld

CXX_ARGS	:=	-g -O3

CFLAGS = -ffreestanding -mcmodel=large -mno-red-zone -nostdlib -lgcc -g
ASFLAGS = -felf64 -F dwarf -g

version		:=	3.0.0
SO_NAME		:=	ozone-$(version)
SO			:=	$(BIN_DIR)/$(SO_NAME).bin

ISO			:=	$(BIN_DIR)/$(SO_NAME).iso

.PHONY: clean test disk iso

all: $(SO)
test: $(ISO)
	@echo Starting Emulation
	@qemu-system-x86_64 -cdrom $(ISO)
dbg: clean $(ISO)
	@echo Starting Debug
	@gnome-terminal -- gdb $(SO) --eval-command="target remote localhost:3117" &
	@qemu-system-x86_64 -cdrom $(ISO) -gdb tcp::3117 -S


$(SO): $(linker) $(OBJFILES)
	@echo Creating $@
	@$(CXXC) -T $(linker) -o $@ $(OBJFILES) -z max-page-size=0x1000 $(CFLAGS) -no-pie

$(OBJ_DIR)/%.s.o: $(SRC_DIR)/%.s
	@echo Creating $@
	@$(CXXC) -c $(CFLAGS) $< -o $@ 

$(OBJ_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	@echo Creating $@
	@$(CXXC) -c $(CFLAGS) $< -o $@ 

clean:
	@echo Cleaning Object Files
	@-rm $(OBJ_DIR)/*.o

disk: $(ISO)
	@echo I\'m going to write the ISO on /dev/sdb
	@read -r -p "Press ENTER to continue"
	@sudo dd if=$(ISO) of=/dev/sdb && sync

$(ISO): $(SO)
	@echo Creaning $@
	@-rm $(BIN_DIR)/isodir/boot/*.bin $(ISO)
	@cp $(SO) $(BIN_DIR)/isodir/boot/
	@grub-mkrescue -o $(ISO) $(BIN_DIR)/isodir