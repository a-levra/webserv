#*******************************  VARIABLES  **********************************#

NAME			=	webserv

# --------------- FILES --------------- #

include src.mk

# ------------ DIRECTORIES ------------ #

DIR_BUILD		=	.build/
DIR_SRC 		=	src/
DIR_INCLUDE		=	include/

# ------------- SHORTCUTS ------------- #

OBJ				=	$(patsubst %.cpp, $(DIR_BUILD)%.o, $(SRC))
DEP				=	$(patsubst %.cpp, $(DIR_BUILD)%.d, $(SRC))
SRC				=	$(addprefix $(DIR_SRC),$(LIST_SRC))

# ------------ COMPILATION ------------ #

CPPFLAGS		=	-Wall -Wextra -Werror -std=c++98

DEP_FLAGS		=	-MMD -MP

# -------------  COMMANDS ------------- #

RM				=	rm -rf
MKDIR			=	mkdir -p

#***********************************  RULES  **********************************#

.PHONY: all
all:			$(NAME)

# ---------- VARIABLES RULES ---------- #

$(NAME):		$(OBJ)
				$(CXX) $(CPPFLAGS) -o $(NAME) $(OBJ)

# ---------- COMPILED RULES ----------- #

-include $(DEP)

$(DIR_BUILD)%.o: %.cpp
				mkdir -p $(shell dirname $@)
				$(CXX) $(CPPFLAGS) $(DEP_FLAGS) -I $(DIR_INCLUDE) -c $< -o $@

.PHONY: clean
clean:
				$(RM) $(DIR_BUILD)

.PHONY: fclean
fclean: clean
				$(RM) $(NAME)

.PHONY: re
re:				fclean
				$(MAKE) all

.PHONY: build_image
build_image: Dockerfile
	docker build -t webserv .

.PHONY: run_container
run_container:
	docker run -p 8000:8000 -p 5000:5000 -v .:/app -it webserv