all: SPLflix

# Tool invocation
# Executable SPLflix
SPLflix: \
	bin/Action.o \
	bin/Session.o \
	bin/User.o \
	bin/Watchable.o \
	bin/Main.o \
	Linking \


bin/Action.o: src/Action.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Include -o bin/Action.o src/Action.cpp

bin/Session.o: src/Session.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Include -o bin/Session.o src/Session.cpp

bin/User.o: src/User.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Include -o bin/User.o src/User.cpp

bin/Watchable.o: src/Watchable.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Include -o bin/Watchable.o src/Watchable.cpp

bin/Main.o: src/Main.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Include -o bin/Main.o src/Main.cpp
	
Linking:
	g++ -o bin/splflix bin/Main.o bin/Action.o bin/Session.o bin/User.o bin/Watchable.o

#Clean the build directory
clean:
	rm -rf bin/*
