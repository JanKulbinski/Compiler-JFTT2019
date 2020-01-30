Uwaga:
Aby program 6-mod-mult.imp działał poprawnie dla podanych w nim danych testowych, powinien być on uruchomiony na maszynie w wersji cln dla dowolnych liczb całkowitych. Jest to spowodowane rozpoczęciem algorytmu dzielenia i modulo liczb a i b, od pomnozenia b razy 2 ^ log(a).

Pliki:  
loops.cpp - funkcje odpowiedzialne za pętle
expressions.cpp - funkcje odpowiedzialne za działania arytmetyczne
conditions.cpp	- funkcje odpowiedzialne za wyrażenia warunkowe
compiler.cpp - funkcje pomocnicze
compiler.hpp - deklaracja globalnych funkcji, zmiennych, struktur
bison.y - parser
lexer.l - lekser
Makefile - kompiluje całość

Testowane w środowisku:
bison --version : 3.0.4
flex --version : 2.6.4
g++ --version : 7.4.0

Uruchomienie:
./kompilator input output

Autor: Jan Kulbiński
Numer indeksu: 244934
