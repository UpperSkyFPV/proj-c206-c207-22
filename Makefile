TARGET_EXEC ?= a.out

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src ./vendor/printf

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s) \
		./vendor/sqlite/sqlite3.c \
		./vendor/loguru/loguru.cpp
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d) \
			./vendor/sqlite \
			./vendor/loguru \
			./vendor/tl/include \
			./vendor/fmt/include \
			./vendor/eventpp/include \
			./vendor/sockpp/include \
			./vendor/cppack/include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

DEBUGFLAGS ?= -g
RELEASEFLAGS ?= -O2 -Werror
CMMFLAGS ?= $(INC_FLAGS) -MMD -MP $(RELEASEFLAGS) -Wall
CPPFLAGS ?= -std=c++20 -DLOGURU_USE_FMTLIB=1
LDFLAGS ?= -lpthread -ldl -lm -L./vendor/fmt/build -lfmt -L./vendor/sockpp/build -lsockpp

CXX = clang++
CC = clang

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CMMFLAGS) $(CFLAGS) $(CFLAGS) -c $< -o $@

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CMMFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p

