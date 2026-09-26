APPNAME=Overflow
INCLUDE=../include/core ../include/game ../include/registries ../include/ui

include ../../common/Makefile

HEADERX += $(shell find ../include -type f -name '*.hpp')

MOC_FILES := $(shell find . -type f -name 'moc_*.cpp')
INPUTFX := $(filter-out $(MOC_FILES),$(INPUTFX))

FLAG += $(shell pkg-config --cflags Qt5Widgets) -fPIC
LIBS += $(shell pkg-config --libs Qt5Widgets)
