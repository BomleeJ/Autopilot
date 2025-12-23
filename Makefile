MAKEFILE_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
XPLANE_SDK   := $(MAKEFILE_DIR)/SDK

CXX = clang++

CXXFLAGS = -std=c++17 -fPIC -O2 \
           -arch x86_64 \
           -DAPL=1 -DXPLM200 -DXPLM210 -DXPLM300 -DXPLM301 \
           -I$(XPLANE_SDK)/CHeaders/XPLM \
           -I$(XPLANE_SDK)/CHeaders/Widgets \
           -I$(shell brew --prefix nlohmann-json)/include -I$(MAKEFILE_DIR)/include

LDFLAGS = -arch x86_64 \
          -F/Users/omarjingoisemperor/Desktop/XplaneDev/SDK/Libraries/Mac \
          -framework XPLM \
          -framework XPWidgets \
          -framework OpenGL \
          -framework CoreFoundation

TARGET = plugin.xpl
SRC = test.cpp
IMPLFiles = $(wildcard src/*.cpp)

all: $(TARGET)
	
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(IMPLFiles) -bundle $(LDFLAGS) -o $(TARGET)
clean:
	rm -f *.o *.xpl
