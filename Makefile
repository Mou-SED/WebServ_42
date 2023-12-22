# ******************************* Documentation ********************************
# 1.Rules
# 	- all: Default rule. Compiles the program.
# 	- clean: Removes all object files and debug files.
# 	- fclean: Removes all object files, debug files, and the executable.
# 	- re: Removes all object files, debug files, executable, and recompiles the
# 		program.
# 	- test: Compiles and runs the test suite for the specified unit.
# 		- Note: this rule expects a make argument called UNIT.
# 	- testall: Compiles and runs all test suites.

# 2.Make arguments
# 	2.1.Build mode arguments
# 		- LEAKS: Compiles the program in leak check mode (-g & -D LEAKS)
# 			- Note: This mode uses 'leaks' command from MacOS, hence it is only
# 				available on MacOS.
# 		- DEBUG: Compiles the program in debug mode (-g only).
# 		- RELEASE: Compiles the program in release mode (-Ofast & -flto).
# 		- Note: The default mode is development mode (-g & -fsanitize=address).

# 	2.2.Other arguments
# 		- RUN: If set, the program will be run after compilation.
# 			- Note: This argument is meant to be used with the all rule only!

# 3.Custom configuration

# 	3.1.Directories section
# 		- SRC_DIR: The directory containing the source files.
# 		- OBJ_DIR: The directory where the object files will be stored.
# 		- INC_DIR: The directory containing the header files.
# 		- TEST_DIR: The directory containing the test files.

# 	3.2.Files section
# 		- NAME: The name of the executable.
# 		- SRCS: This variable uses the find command to find all source files in
# 			the SRC_DIR directory. This should be changed to a static list of
# 			sources files before pushing to 42 intra.

# 	3.3.Compiler options section
# 		- CXX: The compiler to use.
# 		- CXXFLAGS: The compiler flags to use.
# 			- Note: -MMD is used to generate header dependencies.

# 	3.4.Make flags section
# 		- MAKEFLAGS: The make flags to use.
# 			- Note: --silent is used to silence the make output.

#	3.5.Test configuration section
#		- TEST_NAMES: A list of all test suite names.
#			- Note: Please make sure to add all test suite names here.
#			- Note: This is used by the testall rule.
#		- $(UNIT)_DEP: A list of external dependencies for the specified unit.
#			- Note: Please make sure to add all external dependencies here.

# ****************************** Colors & Styles *******************************
NC := \033[m
RED := \033[0;31m
GREEN := \033[1;32m
BLUE := \033[0;34m

# ******************************** Directories *********************************
SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include
TOOL_DIR := tools

# ************************************ Files ***********************************
NAME := Webserv
SRCS := $(shell find $(SRC_DIR) -type f -name *.cpp -print)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# ****************************** Compiler Options ******************************
CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -MMD -I $(INC_DIR)
ifeq ($(shell uname), Darwin)
	CXXFLAGS += -std=c++98
else
	CXXFLAGS += -std=c++11
endif

# ******************************** Make Flags **********************************
MAKEFLAGS += --silent

# ******************************* Main Targets *********************************
all: $(NAME)
	[ -n "$(RUN)" ] && ./$< $(CONFIG) || echo "$(GREEN) ***** Done! ***** $(NC)"

$(NAME): $(OBJS)
	echo "Linking $(BLUE)$(@F)$(NC)..."
	$(CXX) $(LDFLAGS) $^ -o $@
	./$(TOOL_DIR)/clear-line.sh

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	echo "Compiling $(GREEN)$(<F)$(NC)..."
	$(CXX) $(CXXFLAGS) -c $< -o $@
	./$(TOOL_DIR)/clear-line.sh

$(OBJ_DIR):
	mkdir $@

# ****************************** Global Targets ********************************
clean:
	echo "$(RED)Cleaning object files...$(NC)"
	rm -rf $(OBJ_DIR)
	./$(TOOL_DIR)/clear-line.sh
	echo "$(RED)Cleaning debug files...$(NC)"
	rm -f *.dSYM
	./$(TOOL_DIR)/clear-line.sh

fclean: clean
	echo "$(RED)Cleaning $(NAME)...$(NC)"
	./$(TOOL_DIR)/clear-line.sh

re: fclean all

# **************************** Header dependencies *****************************
-include $(patsubst %.o, %.d, $(OBJS))

.PHONY: all clean fclean re
