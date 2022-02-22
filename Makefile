# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dtawney <marvin@42.fr>                     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/11/07 16:45:25 by dtawney           #+#    #+#              #
#    Updated: 2021/11/07 16:45:27 by dtawney          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
HDRS = Server.hpp User.hpp Channel.hpp Defines.hpp
SRCS = main.cpp Server.cpp User.cpp Channel.cpp
OBJS = $(SRCS:.cpp=.o)
CXX  = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98

.PHONY:	all clean fclean re

all:		$(NAME)

.cpp.o:
			$(CXX) $(CPPFLAGS) -c $< -o $@

$(OBJS):	$(HDRS)

$(NAME):	$(OBJS) $(HDRS)
			$(CXX) $(OBJS) $(CPPFLAGS) -o $(NAME)

clean:
			rm -f $(OBJS)

fclean:		clean
			rm -f $(NAME)

re: fclean all
