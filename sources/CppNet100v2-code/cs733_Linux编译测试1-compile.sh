cd `dirname $0`
#

gcc -c ../depends/sqlite/sqlite3.c
gcc -c ../depends/sqlite/shell.c
g++ -c ../depends/sqlite/CppSQLite3.cpp

g++ \
main.cpp \
../depends/json/cJSON.c \
../depends/json/CJsonObject.cpp \
-I ../depends/include \
-I ../depends/json \
-I ../depends/sqlite \
sqlite3.o CppSQLite3.o \
-std=c++11 -pthread -ldl \
-o start
