g++ -std=c++14 -c 1905001_classes.cpp -o 1905001_classes.o
g++ -std=c++14 -c 1905001_main.cpp -o 1905001_main.o
g++ -std=c++14 1905001_classes.o 1905001_main.o -o demo.exe -lfreeglut -lglew32 -lopengl32 -lglu32 && .\demo.exe %1