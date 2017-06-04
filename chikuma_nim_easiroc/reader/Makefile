COMP_NAME = NIMEASIROCReader

LDADD = $(top_builddir)/src/libyaml.la
all: $(COMP_NAME)Comp

SRCS += $(COMP_NAME).cpp
SRCS += $(COMP_NAME)Comp.cpp
#SRCS += easirocdevice.o easiroc_config.o FileManager.o ParameterManager.o StringManager.o

# Socket library
LDLIBS += -L$(DAQMW_LIB_DIR) -lSock

# sample install target
#
# MODE = 0755
# BINDIR = /home/daq/bin
#
# install: $(COMP_NAME)Comp
#	mkdir -p $(BINDIR)
#	install -m $(MODE) $(COMP_NAME)Comp $(BINDIR)

include /usr/share/daqmw/mk/comp.mk
