all: X-IvAp

OS:=$(shell uname)

TOP=../..
WD=$(shell cd ${TOP};echo `pwd`)

# X-IvAp source files
cpp_SRC+=${WD}/src/XIvAp/atcPosition.cpp
cpp_SRC+=${WD}/src/XIvAp/configFile.cpp
cpp_SRC+=${WD}/src/XIvAp/helpers.cpp
cpp_SRC+=${WD}/src/XIvAp/connectForm.cpp
cpp_SRC+=${WD}/src/XIvAp/disconnectForm.cpp
cpp_SRC+=${WD}/src/XIvAp/flightplanForm.cpp
cpp_SRC+=${WD}/src/XIvAp/fsdapi.cpp
cpp_SRC+=${WD}/src/XIvAp/fsdmessage.cpp
cpp_SRC+=${WD}/src/XIvAp/IVAOsecurity.cpp
cpp_SRC+=${WD}/src/XIvAp/systemcallbacks.cpp
cpp_SRC+=${WD}/src/XIvAp/tcpsocket.cpp
#cpp_SRC+=${WD}/src/XIvAp/TeamSpeak.cpp
cpp_SRC+=${WD}/src/XIvAp/uiWindow.cpp
cpp_SRC+=${WD}/src/XIvAp/weatherGod.cpp
cpp_SRC+=${WD}/src/XIvAp/weatherPosition.cpp
cpp_SRC+=${WD}/src/XIvAp/xivap.cpp
cpp_SRC+=${WD}/src/XIvAp/interpolator.cpp
cpp_SRC+=${WD}/src/XIvAp/multiplayer.cpp
cpp_SRC+=${WD}/src/XIvAp/multiplayerPilot.cpp
cpp_SRC+=${WD}/src/XIvAp/HTTPclient.cpp
cpp_SRC+=${WD}/src/XIvAp/messageWindow.cpp
cpp_SRC+=${WD}/src/XIvAp/chatWindow.cpp
cpp_SRC+=${WD}/src/XIvAp/awyFinder.cpp
cpp_SRC+=${WD}/src/XIvAp/aircraftDB.cpp
cpp_SRC+=${WD}/src/XIvAp/airlinesDB.cpp
cpp_SRC+=${WD}/src/XIvAp/FMCform.cpp
cpp_SRC+=${WD}/src/XIvAp/XPmessageBox.cpp
cpp_SRC+=${WD}/src/XIvAp/refInspector.cpp
cpp_SRC+=${WD}/src/XIvAp/TLVpacket.cpp
cpp_SRC+=${WD}/src/XIvAp/messageHandler.cpp
cpp_SRC+=${WD}/src/XIvAp/navQuery.cpp
cpp_SRC+=${WD}/src/XIvAp/WristWatch.cpp
cpp_SRC+=${WD}/src/XIvAp/planeposition.cpp

cpp_SRC+=${WD}/src/stund/StunClient.cpp


# source files from the multiplayer SDK
cpp_SRC+=${WD}/SDK/multiplayer/src/BitmapUtils.cpp
#cpp_SRC+=${WD}/SDK/multiplayer/src/PlatformUtils.lin.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/TexUtils.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XObjDefs.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XObjReadWrite.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XOGLUtils.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XPMPMultiplayer.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XPMPMultiplayerCSL.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XPMPMultiplayerObj.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XPMPMultiplayerVars.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XPMPPlaneRenderer.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XUtils.cpp
# cpp_SRC+=${WD}/SDK/multiplayer/src/CSLLoaderThread.cpp


OBJS+=$(c_SRC:.c=.c.o)

OBJS+=$(cpp_SRC:.cpp=.cpp.o)

CFLAGS+= -iquote. -iquote.. -iquote../.. -iquote../../.. \
 -iquote../../SDK/CHeaders/XPLM \
 -iquote../../SDK/CHeaders/Widgets \
 -iquote../XIvAp \
 -iquote../ptypes/include \
 -iquote../stund \
 -iquote../../SDK \
 -iquote../../SDK/multiplayer \
 -iquote../../SDK/multiplayer/src \
 -iquote../../SDK/multiplayer/include \
 -I/usr/include \

DBG=-g

CFLAGS+=-Wall -w -x c++ -ansi

LIBS+=${WD}/src/stund/stun.o
LIBS+=${WD}/src/stund/udp.o

#LIBS+=-lGL -lGLU -L${WD}/src/ptypes/lib -lptypesn -lpng
#LIBS+=-L${WD}/src/ptypes/lib -L${WD}/src/png/ -lptypes -lpng -lz
LIBS+=-lz ${WD}/src/ptypes/lib/libptypes.a

XIVAP_DEPENDS=ptypes stund png

# Handle Linux/OS X specific stuff here.
ifeq (${OS}, Linux)
	LIBS+=-lpng
	cpp_SRC+=${WD}/SDK/multiplayer/src/PlatformUtils.lin.cpp
	CFLAGS+=-m32 -DLINUX -DLIN=1 -Dlinux -O3 -Wall -fno-strict-aliasing
	LDFLAGS+=-shared -rdynamic -nodefaultlibs -fvisibility=hidden -lGL -lGLU -m32
	LIBS+=../../WidgetLibrary/Binaries/LinuxGCC/XPWidgetsEx.lib
	# link libstdc++ statically so we don't conflict with ATI OpenGL
	# this may require a libstdc++.a or gcc 4.0, not sure which
	LIBS+=-Wl,-Bstatic -lstdc++ -Wl,-Bdynamic
	LIBS+=-Wl,--version-script=${WD}/src/Linux/linker.script
	FINAL=-MMD

	# Gcc 3.4 doesn't seem to like -iquote. We'll assume linux systems to be 3.x, or if thay are
	# 4.x, able to tollerate -I includes.
	CFLAGS := $(subst -iquote,-I,$(CFLAGS))
	XIVAP_DEPENDS+= XPWidgetsEx_lib
endif

ifeq (${OS}, Darwin)
	LIBS+=${WD}/src/png/libpng.a
	cpp_SRC+=${WD}/SDK/XPListBox.cpp
	cpp_SRC+=${WD}/SDK/XPPopups.cpp
	cpp_SRC+=${WD}/SDK/multiplayer/src/PlatformUtils.mac.cpp
	# teamspeex can be driven by URL
	cpp_SRC+=${WD}/src/XIvAp/TeamSpeak.cpp
	CFLAGS+=-DAPPLE -DAPL=1 -DLIN=0 -DIBM=0 -O3 \
	 -I/Developer/Headers/FlatCarbon/ \
	 -I../MacIncludes \
	  -O3 -funsigned-char \
	  -arch ppc -arch i386 \
	  -isysroot /Developer/SDKs/MacOSX10.4u.sdk 
	LDFLAGS+=-flat_namespace -undefined suppress
	LDFLAGS+=-bundle -framework Carbon -framework OpenGL -framework ApplicationServices
	LDFLAGS+= -arch ppc -arch i386
	LDFLAGS+= -Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk
	FINAL=
endif


clean:
	rm -f ${OBJS}
	rm -f Release/Resources/plugins/X-IvAp.xpl

# This cleans the supporting libraries too.
mrproper: clean
	cd ${WD}/src/ptypes/ ; ${MAKE} clean
	cd ${WD}/src/png/ ; ${MAKE} clean
	cd ${WD}/src/stund/ ; ${MAKE} clean
	rm -f ${WD}/src/png/Makefile

X-IvAp: $(XIVAP_DEPENDS)
	$(MAKE) X-IvAp.xpl TARGET=X-IvAp.xpl\
	CC="g++"  LD="g++"  AR="ar -crs"  SIZE="size"

X-IvAp.xpl: ${OBJS}
	${CC} ${LDFLAGS} -o Release/Resources/plugins/X-IvAp.xpl ${OBJS} ${LIBS}

%.cpp.o: %.cpp
	gcc -c -fPIC ${CFLAGS} $< -o $@ ${FINAL}

# Supporting libraries:
ptypes:	${WD}/src/ptypes/lib/libptypes.a

stund:
	@echo "Building stun objects"
	cd ${WD}/src/stund/ ; ${MAKE}

${WD}/src/ptypes/lib/libptypes.a:
	@echo "Building ptypes library"
	cd ${WD}/src/ptypes/ ; ${MAKE}

png: ${WD}/src/png/libpng.a

${WD}/src/png/libpng.a:
ifeq (${OS}, Darwin)
	cp ${WD}/src/png/scripts/makefile.darwin ${WD}/src/png/Makefile
endif
ifeq (${OS}, Linux)
	cp ${WD}/src/png/scripts/makefile.linux ${WD}/src/png/Makefile
endif
	cd ${WD}/src/png/ ; ${MAKE}

XPWidgetsEx_lib:
	@echo "Building WidgetLibrary objects"
	cd ${WD}/WidgetLibrary/Projects/LinuxGCC ; ${MAKE}
