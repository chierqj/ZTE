# bin/bash
rm -fr main
g++ -std=c++11 main.cpp -o main
./main < ../data/test1.txt