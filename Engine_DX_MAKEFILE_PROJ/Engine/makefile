CPP_FILES := $(wildcard Sources/*.cpp)
OBJ_FILES := $(addprefix Objects/,$(notdir $(CPP_FILES:.cpp=.o)))
CC_FLAGS :=  -Wall -O2 -s -DNDEBUG -std=c++11 -mwindows -g -lm -D__NO_INLINE__
LD_FLAGS :=   -ld3d9 -ld3dx9 -lwinmm -lole32 -ldxguid -ldinput -ldinput8

Build/Start: $(OBJ_FILES)
	g++  -o $@ 	$^ 		$(LD_FLAGS)
Objects/%.o: Sources/%.cpp
	g++  -c -o 	$@ $<	$(CC_FLAGS)
Clean:
	rm -rf Objects/*.o Build/Start
