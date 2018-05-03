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
LEVELS    := level/StageTutPnP.o level/StageTutPnPWALLS.o level/Stage1Color.o level/Stage1ColorWalls.o level/Stage2ColorCombo.o level/Stage2ColorComboWALL.o level/Stage3NumberCombo.o level/Stage3NumberComboWalls.o level/Stage4.o level/Stage4Walls.o level/Stage5new.o level/Stage5newWalls.o level/Stage6Both.o level/Stage6BothWalls.o level/Stage7Both.o level/Stage7BothWalls.o level/Stage9Both.o level/Stage9Walls.o
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