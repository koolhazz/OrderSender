#SHELL = /bin/bash

RED = \\e[1m\\e[31m
DARKRED = \\e[31m 
GREEN = \\e[1m\\e[32m
DARKGREEN = \\e[32m 
BLUE = \\e[1m\\e[34m
DARKBLUE = \\e[34m 
YELLOW = \\e[1m\\e[33m
DARKYELLOW = \\e[33m 
MAGENTA = \\e[1m\\e[35m
DARKMAGENTA = \\e[35m 
CYAN = \\e[1m\\e[36m
DARKCYAN = \\e[36m 
RESET = \\e[m
CRESET =  ;echo -ne \\e[m; test -s $@


CC = gcc
CXX = g++

CFLAGS = -Wall -O2 -D_DAEMON_ 
CXXFLAGS = $(CFLAGS)

INCLUDE = inc 

#PATH_INC = $(foreach tmp, inc lib /usr/local/mysql/include, -I $(tmp))
PATH_INC = $(foreach tmp, $(INCLUDE), -I $(tmp)) -I JsonCpp/include

PATH_BIN = bin/
PATH_LIB = -L /usr/local/lib -L JsonCpp/ 

STATIC_LIB = hiredis/libhiredis.a JsonCpp/libjson.a

#print:
#	echo $(PATH_LIB)

HEAD = $(wildcard inc/*.h)

SOURCE = $(wildcard src/*.cc)

OBJ = $(SOURCE:.cc=.o)

LINK = -lz -lcrypto -lcurl

TARGET = UdpLogReport 

all:$(TARGET)
$(TARGET):$(OBJ) $(SOURCE)
	@echo -e $(YELLOW) [Linking...] $(MAGENTA) $@...   $(RESET) $(RED)
	@$(CXX) $(CXXFLAGS) -o $(PATH_BIN)$@ $(OBJ) $(LINK) $(PATH_LIB) $(STATIC_LIB) 
	@echo -e $(RESET)

$(OBJ):%.o:%.cc $(HEAD)
	@echo -e $(YELLOW) [Compling...] $(GREEN)$<...$(RESET) $(RED)
	@$(CXX) $(CXXFLAGS) -c $< -o $@  $(PATH_INC) 
	@echo -e $(RESET)

backup:
	@tar zcvf ../`date +"%F"`.tar.gz ../DealServer/

clean:
	@rm -f $(PATH_BIN)$(TARGET) $(OBJ)
