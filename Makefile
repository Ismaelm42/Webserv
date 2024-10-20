NAME = webserv

SRC =	srcs/main.cpp			\
		srcs/Cgi.cpp			\
		srcs/Client.cpp			\
		srcs/Configuration.cpp	\
		srcs/MimeType.cpp		\
		srcs/Request.cpp		\
		srcs/Response.cpp		\
		srcs/Server.cpp			\
		srcs/Webserv.cpp		\

HEADERS =	includes/lib.hpp			\
			includes/common.hpp			\
			includes/Cgi.hpp			\
			includes/Client.hpp			\
			includes/Configuration.hpp	\
			includes/error.hpp			\
			includes/MimeType.hpp		\
			includes/Request.hpp		\
			includes/Response.hpp		\
			includes/Server.hpp			\
			includes/Webserv.hpp		\


OBJT_DIR = .objt/

OBJT = $(addprefix $(OBJT_DIR), $(patsubst %.cpp, %.o, $(SRC)))

C++ = c++

CFLAGS = -Wall -Wextra -Werror

C++FLAG = -std=c++98

RM = rm -f

MAKEFLAGS += --quiet

all: $(NAME)

$(NAME): $(OBJT)
	${C++} $(CFLAGS) $(C++FLAG) $(OBJT) -o ${NAME}

$(OBJT_DIR)%.o: %.cpp $(HEADERS)
	mkdir -p $(@D)
	$(C++) $(CFLAGS) $(C++FLAG) -c $< -o $@

clean:
	$(RM) -r $(OBJT_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY : all clean fclean re
