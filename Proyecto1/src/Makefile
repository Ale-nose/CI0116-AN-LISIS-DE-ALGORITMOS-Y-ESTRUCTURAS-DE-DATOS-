APPNAME=Overflow
INCLUDE=../include/core ../include/game ../include/registries ../include/ui

include ../../common/Makefile

HEADERX += $(shell find ../include -type f -name '*.hpp')

FLAG += $(shell pkg-config --cflags Qt5Widgets) -fPIC
LIBS += $(shell pkg-config --libs Qt5Widgets)
