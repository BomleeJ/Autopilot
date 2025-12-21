MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
XPLANE_SDK   := $(MAKEFILE_DIR)/SDK

CXX = clang++

CXXFLAGS = -std=c++17 -fPIC -O2 \
           -DAPL=1 -DXPLM200 -DXPLM210 -DXPLM300 -DXPLM301 \
           -I$(XPLANE_SDK)/CHeaders/XPLM \
           -I$(XPLANE_SDK)/CHeaders/Widgets

# LDFLAGS = -L$(XPLANE_SDK)/Libraries/Mac/XPLM.framework  -L$(XPLANE_SDK)/Libraries/Mac/XPWidgets.framework
LDFLAGS = -F/Users/omarjingoisemperor/Desktop/XplaneDev/SDK/Libraries/Mac \
  -framework XPLM \
  -framework XPWidgets \
  -framework OpenGL \

TARGET = plugin.xpl
SRC = test.cpp


all: $(TARGET)
	
$(TARGET): plugin.o
	$(CXX) -bundle plugin.o $(LDFLAGS) -o $(TARGET)

plugin.o: $(SRC)
	$(CXX) -c $(CXXFLAGS) $(SRC) -o plugin.o

clean:
	rm -f *.o *.xpl
