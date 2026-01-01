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
datarefs = datarefs.json
navigation = navigation.json
SRC = test.cpp
IMPLFiles = $(wildcard src/*.cpp)

all: $(TARGET)
	
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(IMPLFiles)  -bundle $(LDFLAGS) -o $(TARGET)

install: $(TARGET)
	cp $(TARGET) "$(HOME)/Library/Application Support/Steam/steamapps/common/X-Plane 11/Resources/plugins"
	cp $(datarefs) "$(HOME)/Library/Application Support/Steam/steamapps/common/X-Plane 11"
	cp $(navigation) "$(HOME)/Library/Application Support/Steam/steamapps/common/X-Plane 11"
clean:
	rm -f *.o *.xpl
