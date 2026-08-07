# Else exist specifically for clang
ifeq ($(CXX),g++)
    EXTRA_FLAGS = --no-gnu-unique
else
    EXTRA_FLAGS =
endif

CXXFLAGS ?= -O2
CXXFLAGS += -shared -fPIC -std=c++2b

SRC = main.cpp CBoxShadowsDecoration.cpp CBoxShadowsPassElement.cpp shadowsLua.cpp geometry.cpp
DEPS = pixman-1 libdrm hyprland pangocairo libinput libudev wayland-server xkbcommon

all:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) $(SRC) -o shadows-plus-plus.so `pkg-config --cflags $(DEPS)`

test:
	$(CXX) -std=c++2b -I. $(LDFLAGS) geometry.cpp tests/shadowsLua.cpp tests/geometry.cpp -o shadows-plus-plus-tests `pkg-config --cflags hyprutils gtest` `pkg-config --libs hyprutils gtest` -lgtest_main
	./shadows-plus-plus-tests

clean:
	rm -f ./shadows-plus-plus.so ./shadows-plus-plus-tests
