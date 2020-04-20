# bin/bash
# project(\"${project}\")\n

INPUT="../data/case$1/test.txt"
OUTPUT="../data/case$1/result.txt"

cd bin
rm -fr main
cd ../src
g++ -std=c++11 main.cpp -o main -D TEST
mv main ../bin
cd ../bin
./main < $INPUT > $OUTPUT
cd ../judge
java -jar judge.jar $INPUT $OUTPUT