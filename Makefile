NAME		=	exe

CXX			=	g++
CXXFLAGS	=	-std=c++17 -Wall -Wextra -Werror -g3

SRCS		=	main.cpp Board.cpp GameAnalyzer.cpp Pieces.cpp

OBJS		=	$(SRCS:%.cpp=%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re