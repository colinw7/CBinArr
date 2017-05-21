CC = g++
AR = ar
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/CBinArr

SRC = \
CBinArr.cpp \

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-I. \
-I$(INC_DIR) \
-I../../CStrUtil/include \

LFLAGS = \
-L$(LIB_DIR) \
-L../../CStrUtil/lib \

LIBS = -lCStrUtil

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CBinArr

$(OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CC) $(CDEBUG) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

.SUFFIXES: .cpp

$(BIN_DIR)/CBinArr: $(OBJ_DIR)/CBinArr.o
	$(CC) $(LDEBUG) -o $(BIN_DIR)/CBinArr $(OBJ_DIR)/CBinArr.o $(LFLAGS) $(LIBS)
