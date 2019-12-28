#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <climits>
#include <memory>
#include <math.h>
#include <vector>
#include <stack>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <algorithm>


using namespace std;

typedef struct {
	string name;
    string type; //NUM, IDE, ARR
    int initialized;
    int counter;
	long long int mem;
	long long int local;
  	long long int tableSize;
} Identifier;

void ident(char* variable, int yylineo);
//functions declarations...
