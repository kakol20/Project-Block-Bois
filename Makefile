#
# hello.mak
#
# tilemap text via tonclib
#
# NOTE: for educational purposes only. For real work, use 
# devkitPro's templates ( $(DEVKITPRO)/examples/gba/template )
# or tonclib's templates ( $(TONCCODE)/lab/template )

PATH := $(DEVKITARM)/bin:$(PATH)

# --- Project details -------------------------------------------------

PROJ    := main
TITLE   := $(PROJ)

LIBTONC	 := tonclib
INCLUDE  := -I$(LIBTONC)/include
LIBPATHS := -L$(LIBTONC)/lib
LIBS     := -ltonc

BOXES     := boxes/BoxClear.o boxes/Box1.o boxes/Box2.o boxes/Box3.o boxes/Box4.o boxes/Box5.o boxes/Box6.o boxes/Box7.o boxes/Box8.o boxes/Box9.o
DROPBOXES := dropbox/DBox1clear.o dropbox/DBox2clear.o dropbox/DBox3clear.o dropbox/DBox4clear.o dropbox/DBox5clear.o dropbox/DBox6clear.o dropbox/DBox7clear.o dropbox/DBox8clear.o dropbox/DBox9clear.o dropbox/DBoxclear.o
LEVELS    := levels/StageTutPnP.o levels/StageTutPnPWALLS.o levels/Stage1Color.o levels/Stage1ColorWalls.o levels/Stage2ColorCombo.o levels/Stage2ColorComboWALL.o levels/Stage3NumberCombo.o levels/Stage3NumberComboWalls.o levels/Stage4.o levels/Stage4Walls.o levels/Stage5new.o levels/Stage5newWalls.o levels/Stage6Both.o levels/Stage6BothWalls.o levels/Stage7Both.o levels/Stage7BothWalls.o levels/Stage9Both.o levels/Stage9Walls.o
MISC      := other/EndZone.o player.o other/MovingBlocks.o

COBJS := $(PROJ).o $(BOXES) $(DROPBOXES) $(LEVELS) $(MISC)
OBJS  := $(COBJS)

# --- boot type (MB=0 : normal. MB=1 : multiboot) ---

MB = 0

ifeq ($(MB),1)

TARGET	:= $(PROJ).mb
SPECS	:= -specs=gba_mb.specs

else

TARGET	:= $(PROJ)
SPECS	:= -specs=gba.specs

endif

# --- Compiling -------------------------------------------------------

CROSS	?= arm-none-eabi-
AS		:= $(CROSS)as
CC		:= $(CROSS)gcc
LD		:= $(CROSS)gcc
OBJCOPY	:= $(CROSS)objcopy


ARCH	:= -mthumb-interwork -mthumb

ASFLAGS	:= -mthumb-interwork
CFLAGS	:= $(ARCH) $(INCLUDE) -O2 -Wall -fno-strict-aliasing
LDFLAGS	:= $(ARCH) $(SPECS) $(LIBPATHS) $(LIBS) -Wl,-Map,$(PROJ).map

.PHONY : build clean

# --- Build -----------------------------------------------------------

build : $(TARGET).gba


$(TARGET).gba : $(TARGET).elf
	$(OBJCOPY) -v -O binary $< $@
	-@gbafix $@ -t$(TITLE)

$(TARGET).elf : $(OBJS)
	$(LD) $^ $(LDFLAGS) -o $@

$(COBJS) : %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
# --- Clean -----------------------------------------------------------

clean : 
	@rm -fv *.gba
	@rm -fv *.elf
	@rm -fv *.o

#EOF