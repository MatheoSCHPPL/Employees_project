TARGET = bin/app
SRC = $(wildcard src/*.c)  
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
 
default: $(TARGET)   
	./$(TARGET) -f ./test.db -n 

$(TARGET): $(OBJ)    
	gcc $? -o $(TARGET) 

obj/%.o: src/%.c
	gcc -c $< -o $@ -Iinclude -g	

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db	 
