# bin/bash
rm -fr main
g++ -std=c++11 main.cpp -o main -D TEST
./main < ../data/test2.txt > ../data/result.txt
cd ../judge
java -jar judge.jar ../data/test2.txt ../data/result.txt