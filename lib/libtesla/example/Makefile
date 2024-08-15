#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

#---------------------------------------------------------------------------------
# TARGET ist der Name der Ausgabedatei
# BUILD ist das Verzeichnis, in dem Objektdateien und Zwischenprodukte abgelegt werden
# SOURCES ist eine Liste von Verzeichnissen, die Quellcode enthalten
# DATA ist eine Liste von Verzeichnissen, die Datendateien enthalten
# INCLUDES ist eine Liste von Verzeichnissen, die Header-Dateien enthalten
# ROMFS ist das Verzeichnis, das Daten enthält, die zu RomFS hinzugefügt werden sollen, relativ zum Makefile (Optional)
#
# NO_ICON: Wenn gesetzt, wird kein Icon verwendet.
# NO_NACP: Wenn gesetzt, wird keine .nacp-Datei generiert.
# APP_TITLE ist der Name der App, der in der .nacp-Datei gespeichert wird (Optional)
# APP_AUTHOR ist der Autor der App, der in der .nacp-Datei gespeichert wird (Optional)
# APP_VERSION ist die Version der App, die in der .nacp-Datei gespeichert wird (Optional)
# APP_TITLEID ist die Titel-ID der App, die in der .nacp-Datei gespeichert wird (Optional)
# ICON ist der Dateiname des Icons (.jpg), relativ zum Projektordner.
#   Wenn nicht gesetzt, wird versucht, eine der folgenden Dateien zu verwenden (in dieser Reihenfolge):
#     - <Projektname>.jpg
#     - icon.jpg
#     - <libnx-Ordner>/default_icon.jpg
#
# CONFIG_JSON ist der Dateiname der NPDM-Konfigurationsdatei (.json), relativ zum Projektordner.
#   Wenn nicht gesetzt, wird versucht, eine der folgenden Dateien zu verwenden (in dieser Reihenfolge):
#     - <Projektname>.json
#     - config.json
#   Wenn eine JSON-Datei bereitgestellt oder automatisch erkannt wird, wird ein ExeFS PFS0 (.nsp) anstelle einer Homebrew-Ausführungsdatei (.nro) erstellt. Dies ist für die Verwendung in Systemmodulen gedacht.
#   Die Erstellung der NACP-Datei wird ebenfalls übersprungen.
#---------------------------------------------------------------------------------

APP_TITLE			:=		Tesla Example
APP_VERSION 	:= 		1.3.0

TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCES		:=	source
DATA		:=	data
INCLUDES	:=	../include

NO_ICON		:=  1

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS	:=	-g -Wall -O2 -ffunction-sections \
			$(ARCH) $(DEFINES)

CFLAGS	+=	$(INCLUDE) -D__SWITCH__

CXXFLAGS	:= $(CFLAGS) -fno-exceptions -std=c++20

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= -lnx

#---------------------------------------------------------------------------------
# Liste der Verzeichnisse, die Bibliotheken enthalten; dies muss das oberste Verzeichnis sein, das
# include und lib enthält
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(LIBNX)


#---------------------------------------------------------------------------------
# Ab hier ist normalerweise keine Bearbeitung erforderlich, es sei denn, es müssen zusätzliche
# Regeln für verschiedene Dateierweiterungen hinzugefügt werden
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifeq ($(strip $(CONFIG_JSON)),)
	jsons := $(wildcard *.json)
	ifneq (,$(findstring $(TARGET).json,$(jsons)))
		export APP_JSON := $(TOPDIR)/$(TARGET).json
	else
		ifneq (,$(findstring config.json,$(jsons)))
			export APP_JSON := $(TOPDIR)/config.json
		endif
	endif
else
	export APP_JSON := $(TOPDIR)/$(CONFIG_JSON)
endif

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.jpg)
	ifneq (,$(findstring $(TARGET).jpg,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).jpg
	else
		ifneq (,$(findstring icon.jpg,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.jpg
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_ICON)),)
	export NROFLAGS += --icon=$(APP_ICON)
endif

ifeq ($(strip $(NO_NACP)),)
	export NROFLAGS += --nacp=$(CURDIR)/$(TARGET).nacp
endif

ifneq ($(APP_TITLEID),)
	export NACPFLAGS += --titleid=$(APP_TITLEID)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(CURDIR)/$(ROMFS)
endif

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)


$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@rm -fr $(BUILD) $(TARGET).ovl $(TARGET).nro $(TARGET).nacp $(TARGET).elf


#---------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
all	:	 $(OUTPUT).ovl

$(OUTPUT).ovl		:	$(OUTPUT).elf $(OUTPUT).nacp 
	@elf2nro $< $@ $(NROFLAGS)
	@echo "built ... $(notdir $(OUTPUT).ovl)"

$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES_BIN)

#---------------------------------------------------------------------------------
# Sie benötigen eine Regel wie diese für jede Erweiterung, die Sie als Binärdaten verwenden
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
