mac {
	CONFIG += x86 ppc
	ICON = images/p2ptester.icns
}

win {
	RC_FILE += images/winicon.rc
}

QT += core gui network
TARGET = p2ptester
CONFIG += release warn_on
TEMPLATE = app
FORMS = src/p2ptester.ui
HEADERS = src/p2ptester.h src/downloadagent.h
SOURCES = src/p2ptester.cpp src/downloadagent.cpp
LIBS += ../stund/StunClient.o ../stund/stun.o ../stund/udp.o
OBJECTS_DIR = build
UI_DIR = build
UI_HEADERS_DIR = build
UI_SOURCES_DIR = build
MOC_DIR = build
