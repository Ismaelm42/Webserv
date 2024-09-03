NAME = webserv

SRC =	srcs/main.cpp			\
		srcs/Configuration.cpp	\
		srcs/Client.cpp			\
		srcs/Server.cpp			\
		srcs/Webserv.cpp		\
		srcs/Request.cpp		

HEADERS = includes/Client.hpp			\
		  includes/common.hpp			\
		  includes/Configuration.hpp	\
		  includes/lib.hpp				\
		  includes/Request.hpp			\
		  includes/Server.hpp			\
		  includes/Webserv.hpp			

OBJT_DIR = .objt/

OBJT = $(addprefix $(OBJT_DIR), $(patsubst %.cpp, %.o, $(SRC)))

C++ = c++

CFLAGS = -Wall -Wextra -Werror -D DEBUG=$(DEBUG)

C++FLAG = -std=c++98

RM = rm -f

MAKEFLAGS += --quiet

ifdef DEBUG
        DEBUG = 1
else
        DEBUG = 0
endif

all: $(NAME)

$(NAME): $(OBJT)
	${C++} $(CFLAGS) $(C++FLAG) $(OBJT) -o ${NAME}

$(OBJT_DIR)%.o: %.cpp $(HEADERS)
	mkdir -p $(@D)
	$(C++) $(CFLAGS) $(C++FLAG) -c $< -o $@

debug:
	make re DEBUG=1

clean:
	$(RM) -r $(OBJT_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY : all clean fclean re
