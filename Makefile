.SECONDARY:

DEFINES 		= -DNET02_DEBUG_LOG

INCLUDES 		+= -iquote"./src" 
DBG			= -g
#OPTIMIZE		= -Os
C_FLAGS 		= -Wall $(OPTIMIZE) $(DBG) -w $(DEFINES) $(INCLUDES) -pthread

CXX_FLAGS		= $(C_FLAGS)
CXX_CMD			= g++ $(CXX_FLAGS)

BUILD 			= ./build

#LIB			= -lpthread

OBJECTS 		:= $(patsubst %.cc, $(BUILD)/%.o, $(notdir $(wildcard ./src/*.cc) ) )

DEPENDS			:= $(OBJECTS:.o=.d) 
DEPSDIR			= $(BUILD)
DEP_FLAGS		= -MMD -MP -MF $(patsubst %.o, %.d, $@)

DRIVERS 		= $(notdir $(patsubst %.cc, %, $(wildcard ./driver/*.cc) ) )
TESTS 			= $(notdir $(patsubst %.cc, %, $(wildcard ./test/*.cc) ) )

default: all

.PHONY: all
all: blah

$(BUILD)/%.o: src/%.cc src/%.h src/log.h ./Makefile
	$(CXX_CMD) $(DEP_FLAGS) -c $< -o $@

$(BUILD)/driver/%.o: driver/%.cc $(OBJECTS)
	$(CXX_CMD) $(DEP_FLAGS) -c $< -o $@

$(BUILD)/driver/%: $(BUILD)/driver/%.o $(OBJECTS)
	$(CXX_CMD) $+ $(LIB) -o $@

$(BUILD)/test/%.o: test/%.cc $(OBJECTS)
	$(CXX_CMD) $(DEP_FLAGS) -c $< -o $@

$(BUILD)/test/%: $(BUILD)/test/%.o $(OBJECTS)
	$(CXX_CMD) $+ $(LIB) -o $@

define driver-template
$(1): $$(BUILD)/driver/$(1)
	$(BUILD)/driver/$(1)
endef

.PHONY: $(DRIVERS) 
$(foreach driver, $(DRIVERS), $(eval $(call driver-template,$(driver)) ) )

define test-template
$(1): $$(BUILD)/test/$(1) 
	$(BUILD)/test/$(1)
endef

.PHONY: $(TESTS) 
$(foreach test, $(TESTS), $(eval $(call test-template,$(test)) ) )

.PHONY: clean 
clean: 
	rm -vf $(shell find $(BUILD) -type f -not -name .gitignore )

-include $(DEPENDS)