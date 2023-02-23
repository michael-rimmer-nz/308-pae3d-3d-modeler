CC = g++
IPATH = -I./include
LIBS= -lGL -lglut -lGLU -ljpeg -lpng16
BUILD = build/
SRC = src/
CPP_FILES := $(wildcard src/*.cpp)
O_FILES := $(addprefix $(BUILD),$(notdir $(CPP_FILES:.cpp=.o)))

all: $(BUILD) $(BUILD)PAE3D

$(BUILD):
	mkdir $(BUILD)

$(BUILD)PAE3D: $(O_FILES)
	$(CC) -o $@ $^ $(LIBS)

$(BUILD)%.o: $(SRC)%.cpp
	$(CC) -c -o $@ $^ $(IPATH)

clean:
	$(RM) -f $(BUILD)*.o $(BUILD)PAE3D
