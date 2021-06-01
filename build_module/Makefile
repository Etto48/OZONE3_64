CXXC		:=	g++
AS			:=	as
BIN_DIR		:=	bin
OBJ_DIR		:=	obj
SRC_DIR		:=	src
ISO_DIR		:=	iso


#BOOTSTRAP_FILE:= src/bootstrap.asm
#BOOTSTRAP_OBJ:= obj/$(notdir $(BOOTSTRAP_FILE)).o
HEADERS		:=	$(wildcard $(SRC_DIR)/include/*.h)
ASFILES		:=	$(wildcard $(SRC_DIR)/*.s)
CXXFILES	:=	$(wildcard $(SRC_DIR)/*.cpp)
OBJFILES	:=	$(addprefix $(OBJ_DIR)/,$(patsubst %.s,%.s.o,$(notdir $(ASFILES))) $(patsubst %.cpp,%.cpp.o,$(notdir $(CXXFILES)))) $(BOOTSTRAP_OBJ)
linker		:=	$(SRC_DIR)/linker.ld

CFLAGS = -ffreestanding -mcmodel=large -mno-red-zone -nostdlib -lgcc -g
ASFLAGS = -felf64 -F dwarf -g

MODULE_NAME	:=	module
MODULE		:=	$(BIN_DIR)/$(MODULE_NAME).bin


.PHONY: clean

all: $(MODULE) $(HEADERS)

$(MODULE): $(linker) $(OBJFILES) $(HEADERS)
	@echo Creating $@
	@$(CXXC) -T $(linker) -o $@ $(OBJFILES) -z max-page-size=0x1000 $(CFLAGS) -no-pie

$(OBJ_DIR)/%.s.o: $(SRC_DIR)/%.s $(HEADERS)
	@echo Creating $@
	@$(CXXC) -c $(CFLAGS) $< -o $@ 

$(OBJ_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@echo Creating $@
	@$(CXXC) -c $(CFLAGS) $< -o $@ 

clean:
	@echo Cleaning Object Files
	@-rm $(OBJ_DIR)/*.o
