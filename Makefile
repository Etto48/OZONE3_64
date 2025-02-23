CXX		:=	g++
AS			:=	as
BIN_DIR		:=	bin
OBJ_DIR		:=	obj
SRC_DIR		:=	src
ISO_DIR		:=	iso
LIB_DIR		:=	lib

HEADERS		:=	$(wildcard $(SRC_DIR)/include/*.h) lib/ozone.h
ASFILES		:=	$(wildcard $(SRC_DIR)/*.s)
CXXFILES	:=	$(wildcard $(SRC_DIR)/*.cpp)
OBJFILES	:=	$(addprefix $(OBJ_DIR)/,$(patsubst %.s,%.s.o,$(notdir $(ASFILES))) $(patsubst %.cpp,%.cpp.o,$(notdir $(CXXFILES))))

LIBCXXFILES	:=	$(wildcard $(LIB_DIR)/*.cpp)
LIBASFILES	:=	$(wildcard $(LIB_DIR)/*.s)
LIBOBJFILES	:=	$(addprefix $(OBJ_DIR)/,$(patsubst %.s,%.s.o,$(notdir $(LIBASFILES))) $(patsubst %.cpp,%.cpp.o,$(notdir $(LIBCXXFILES))))

linker		:=	$(SRC_DIR)/linker.ld

CXXFLAGS = -ffreestanding -mcmodel=large -mno-red-zone -nostdlib -lgcc -g -Ilib -static -std=c++2a
ASFLAGS = -felf64 -F dwarf -g

version		:=	3.0.0
SO_NAME		:=	ozone-$(version)
SO			:=	$(BIN_DIR)/$(SO_NAME).bin

LIB			:=	$(OBJ_DIR)/ozone.a

ISO			:=	$(BIN_DIR)/$(SO_NAME).iso



MOD_DIR		:=	build_module
MOD_SRC		:=	$(MOD_DIR)/src
MOD_OBJ		:=	$(MOD_DIR)/obj
MOD_BIN		:=	$(MOD_DIR)/bin
MODHEADERS	:=	$(LIB_DIR)/ozone.h
MODASFILES	:=	$(wildcard $(MOD_SRC)/*.s)
MODCXXFILES	:=	$(wildcard $(MOD_SRC)/*.cpp)
MODOBJFILES	:=	$(addprefix $(MOD_OBJ)/,$(patsubst %.s,%.s.o,$(notdir $(MODASFILES))) $(patsubst %.cpp,%.cpp.o,$(notdir $(MODCXXFILES))))
modlinker	:=	$(MOD_SRC)/linker.ld

MODULES		:=	$(foreach _mod,$(sort $(dir $(wildcard $(MOD_SRC)/modules/*/))), $(lastword $(subst /, ,$(_mod))))

GRUB_CFG	:=	$(BIN_DIR)/isodir/boot/grub/grub.cfg

BINARIES	:=	$(SO) $(foreach _mod,$(MODULES),$(MOD_BIN)/$(_mod).bin)

QEMUARGS	:=	-m 4G -serial stdio #-bios /usr/share/ovmf/OVMF.fd

.PHONY: dbg-server dbg clean test disk iso lib $(MODULES) all_mods $(GRUB_CFG)

all: $(ISO) $(SO) $(HEADERS) $(LIB) $(MODULES)
test: $(ISO)
	@echo Starting Emulation
	@qemu-system-x86_64 $(QEMUARGS) -cdrom $(ISO)

dbg-server: $(ISO)
	@echo Starting Debug Server
	@qemu-system-x86_64 $(QEMUARGS) -cdrom $(ISO) -gdb tcp::3117 -S

dbg: $(ISO)
	@echo Starting Debug
	@gnome-terminal -- gdb $(SO) --eval-command="target remote localhost:3117" &
	@qemu-system-x86_64 $(QEMUARGS) -cdrom $(ISO) -gdb tcp::3117 -S

$(SO): $(linker) $(OBJFILES) $(HEADERS) $(LIB)
	@echo Creating $@
	@$(CXX) -T $(linker) -o $@ $(OBJFILES) $(LIB) -z max-page-size=0x1000 $(CXXFLAGS) -no-pie

$(OBJ_DIR)/%.s.o: $(SRC_DIR)/%.s $(HEADERS)
	@echo Creating $@
	@$(CXX) -c $(CXXFLAGS) $< -o $@ 

$(OBJ_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@echo Creating $@
	@$(CXX) -c $(CXXFLAGS) $< -o $@ 

$(OBJ_DIR)/%.s.o: $(LIB_DIR)/%.s lib/ozone.h
	@echo Creating $@
	@$(CXX) -c $(CXXFLAGS) $< -o $@ 

$(OBJ_DIR)/%.cpp.o: $(LIB_DIR)/%.cpp lib/ozone.h
	@echo Creating $@
	@$(CXX) -c $(CXXFLAGS) $< -o $@ 

clean:
	@echo Cleaning Object Files
	@rm -f $(OBJ_DIR)/*.o
	@rm -f $(BIN_DIR)/*.bin
	@rm -f $(BIN_DIR)/isodir/boot/*.bin
	@rm -f $(MOD_BIN)/*.bin
	@rm -f $(OBJ_DIR)/*.a
	@rm -f $(MOD_OBJ)/*.o

disk: $(ISO)
	@echo I\'m going to write the ISO on /dev/sdb
	@read -r -p "Press ENTER to continue"
	@sudo dd if=$(ISO) of=/dev/sdb && sync

$(ISO): $(SO) $(MODULES) $(HEADERS) $(GRUB_CFG)
	@echo Creaning $@
	@rm -f $(BIN_DIR)/isodir/boot/*.bin $(ISO)
	@cp $(SO) $(BIN_DIR)/isodir/boot/
	@cp $(MOD_BIN)/* $(BIN_DIR)/isodir/boot/
	@grub-mkrescue -o $(ISO) $(BIN_DIR)/isodir 2>&-

$(GRUB_CFG):
	@echo Creating $@
	@echo 'set timeout=0' > $(GRUB_CFG)
	@echo 'set default=0' >> $(GRUB_CFG)
	@echo 'insmod efi_gop' >> $(GRUB_CFG)
	@echo 'menuentry "OZONE3 AMD64" {' >> $(GRUB_CFG)
	@echo '    multiboot /boot/$(SO_NAME).bin' >> $(GRUB_CFG)
#	@echo '    GFXMODE=1024x768' >> $(GRUB_CFG)	
#	@echo '    GFXPAYLOAD=1024x768x32' >> $(GRUB_CFG)
	@for module in $(MODULES) ; do \
        echo "    module /boot/$$module.bin $$module" >> $(GRUB_CFG); \
    done
	@echo '    boot' >> $(GRUB_CFG)
	@echo '}' >> $(GRUB_CFG)
	



lib: $(LIB_DIR)/ozone.a

$(LIB) : $(LIBOBJFILES)
	@echo Creating $@
	@ar r $@ $? 2>&-



all_mods: $(MODULES)

$(MODULES): % : $(MODOBJFILES) $(LIB)
	@echo Creating $(MOD_BIN)/$@.bin
	@$(CXX) -T $(modlinker) -o $@ $(wildcard $(MOD_SRC)/modules/$@/*.cpp) $(wildcard $(MOD_SRC)/modules/$@/*.s) $? -z max-page-size=0x1000 $(CXXFLAGS) -no-pie
	@mv $@ $(MOD_BIN)/$@.bin

$(MOD_OBJ)/%.s.o: $(MOD_SRC)/%.s $(MODHEADERS)
	@echo Creating $@
	@$(CXX) -c $(CXXFLAGS) $< -o $@ 

$(MOD_OBJ)/%.cpp.o: $(MOD_SRC)/%.cpp $(MODHEADERS)
	@echo Creating $@
	@$(CXX) -c $(CXXFLAGS) $< -o $@ 
