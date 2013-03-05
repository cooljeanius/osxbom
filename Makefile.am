NAME=lsbom

CXX=@CXX@

all: $(NAME)

$(NAME): $(NAME).cpp bom.h
	$(CXX) -o $@ $< -I. -Wall -Werror

clean:
	@RM@ -f $(NAME) $(NAME).o
