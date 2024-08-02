NAME = webserv

SRC =	srcs/main.cpp			\
		srcs/Client.cpp			\
		srcs/Server.cpp			\
		srcs/Socket.cpp			\
		srcs/Request.cpp		\
		srcs/Response.cpp		\

OBJT_DIR = .objt/

OBJT = $(addprefix $(OBJT_DIR), $(patsubst %.cpp, %.o, $(SRC)))

C++ = c++

CFLAGS = -Wall -Wextra -Werror

C++FLAG = -std=c++11

RM = rm -f

MAKEFLAGS += --quiet

all: $(NAME)

$(NAME): $(OBJT)
	${C++} $(CFLAGS) $(C++FLAG) $(OBJT) -o ${NAME}

$(OBJT_DIR)%.o: %.cpp
	mkdir -p $(@D)
	$(C++) $(CFLAGS) $(C++FLAG) -c $< -o $@

clean:
	$(RM) -r $(OBJT_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY : all clean fclean re
