NAME = webserv

SRC =	srcs/main.cpp			\
		srcs/Cgi.cpp			\
		srcs/Client.cpp			\
		srcs/Configuration.cpp	\
		srcs/Request.cpp		\
		srcs/Server.cpp			\
		srcs/Webserv.cpp		\
		#srcs/Response.hpp		\#

HEADERS =	includes/lib.hpp			\
			includes/common.hpp			\
			includes/Cgi.hpp			\
			includes/Client.hpp			\
			includes/Configuration.hpp	\
			includes/Request.hpp		\
			includes/Server.hpp			\
			includes/Webserv.hpp		\
			#includes/Response.hpp		\#

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















# NAME = webserv

# SRC =	srcs/main.cpp			\
# 		srcs/Configuration.cpp	\
# 		srcs/Cgi.cpp			\
# 		srcs/Client.cpp			\
# 		srcs/Server.cpp			\
# 		srcs/Request.cpp		\
# 		srcs/Webserv.cpp		\

# OBJT_DIR = .objt/

# OBJT = $(addprefix $(OBJT_DIR), $(patsubst %.cpp, %.o, $(SRC)))

# C++ = c++

# CFLAGS = -Wall -Wextra -Werror -D DEBUG=$(DEBUG)

# C++FLAG = -std=c++98

# RM = rm -f

# MAKEFLAGS += --quiet

# ifdef DEBUG
#         DEBUG = 1
# else
#         DEBUG = 0
# endif

# all: $(NAME)

# $(NAME): $(OBJT)
# 	${C++} $(CFLAGS) $(C++FLAG) $(OBJT) -o ${NAME}

# $(OBJT_DIR)%.o: %.cpp
# 	mkdir -p $(@D)
# 	$(C++) $(CFLAGS) $(C++FLAG) -c $< -o $@

# debug:
# 	make re DEBUG=1

# clean:
# 	$(RM) -r $(OBJT_DIR)

# fclean: clean
# 	$(RM) $(NAME)

# re: fclean all

# .PHONY : all clean fclean re
