#if 0

#########################################
#                                       #
#   primeat for win32                   #
#                                       #
#   by Paul Hsieh                       #
#                                       #
#########################################

APPNAME1 = primeat
CDIR = .
ODIR = .\obj
ADIR = .\arc
CC  = *wcc386
CPP = *wpp386
DEVNULL = nul

SYSLFLAGS = SYS nt
SYSCFLAGS = -bt=nt

CFLAGS = -e25 -zq -otexan -mf -5r $(SYSCFLAGS) -bm -wx -we 
#DEFINES = -DDEBUG -DMEMORY_DEBUG
#INCLUDES = -i=$(CDIR)\hash

# Object files

OFILES1 = $(ODIR)\prime32.obj $(ODIR)\primeat.obj &
          $(ODIR)\main.obj

OFILES2 = 

# Makefile verbose setting

QUIET = @

.c:   $(CDIR)
.cpp: $(CDIR)

# The default build rule for cpp -> obj creation.

.c.obj: .AUTODEPEND
	$(QUIET)echo     Compiling: $[*
	$(QUIET)$(CC) $[* $(INCLUDES) $(CFLAGS) $(DEFINES) -fo=$(ODIR)\

.cpp.obj: .AUTODEPEND
	$(QUIET)echo     Compiling: $[*
	$(QUIET)$(CPP) $[* $(INCLUDES) $(CFLAGS) $(DEFINES) -fo=$(ODIR)\


# The first rule is the default build rule, from which all other rules may be
# activated

all : $(APPNAME1).exe

$(APPNAME1).exe : $(OFILES1) makefile .AUTODEPEND
	$(QUIET)%write $(ODIR)\$(APPNAME1).lnk NAME $(APPNAME1).exe
	$(QUIET)%append $(ODIR)\$(APPNAME1).lnk $(SYSLFLAGS) op m=$(ODIR)\$(APPNAME1).map d all op st=16384 op maxe=25 op q op symf=$(ODIR)\$(APPNAME1).sym op redefsok
	$(QUIET)for %%f in ($(OFILES1)) do @%append $(ODIR)\$(APPNAME1).lnk FIL %%f
	$(QUIET)echo     Linking: $(APPNAME1)
	$(QUIET)*wlink @$(ODIR)\$(APPNAME1).lnk
	$(QUIET)echo.
	$(QUIET)echo     Target $(APPNAME1).exe done.

# Non-default build rules.

# Initial commands invoked on every make

.BEFORE
	$(QUIET)!if not exist $(ODIR)\nul md $(ODIR)
	$(QUIET)echo xxx > $(ODIR)\xxx
	$(QUIET)if not exist $(ODIR)\xxx mkdir $(ODIR)
	$(QUIET)if exist $(ODIR)\xxx del $(ODIR)\xxx > $(DEVNULL)
	$(QUIET)echo     $(APPNAME1)
	$(QUIET)echo     =======
	$(QUIET)set INCLUDE=.;$(%watcom)\h;$(%watcom)\h\nt
	$(QUIET)set PATH=$(%path);$(%watcom)\binnt
	$(QUIET)set DOS4G=QUIET

# User defined commands

clean: .SYMBOLIC
	$(QUIET)echo     Removing build targets
	$(QUIET)for %%f in ($(ODIR)\*.obj *.exe $(ODIR)\*.lnk $(ODIR)\*.map $(ODIR)\*.sym $(APPNAME1).exe *.err) do @if exist %%f del %%f > nul

archive: .SYMBOLIC
	$(QUIET)echo Cleaning
	$(QUIET)echo ========
	$(QUIET)wmake /h clean
	$(QUIET)echo Various archive commands
	$(QUIET)echo ========================
	$(QUIET)!if not exist $(ADIR)\nul md $(ADIR)
	$(QUIET)echo xxx > $(ADIR)\xxx
	$(QUIET)if not exist $(ADIR)\xxx mkdir $(ADIR)
	$(QUIET)if exist $(ADIR)\xxx del $(ADIR)\xxx > $(DEVNULL)
 	$(QUIET)-pkzip -u $(ADIR)\$(APPNAME1).zip *.c *.h makefile > $(DEVNULL)
	$(QUIET)-pkzip -v $(ADIR)\$(APPNAME1).zip

#endif
