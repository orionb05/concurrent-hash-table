# Comiples multiple .c files into chash

SRCS = src/chash.c src/hashtable.c src/rwlock.c src/jenkins.c
TARGET = chash

$(TARGET): $(SRCS)
	gcc -Wall -pthread $(SRCS) -o $(TARGET)

#
clean:
	rm -f $(TARGET)