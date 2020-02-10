# Compiler-JFTT2019

It is a compiler of a simple imperative language, created as a final project for 'Formal languages and translation techniques' course.
The language provides basic features such as:
 * declaration of variables and arrays
 * if/else blocks
 * for, for down, while and do while loops
 * mathematical operations: multiplication, division, addition, subtraction, modulo (mind unwonted definitions of modulo for negative numbers! More info: [task.pdf](task.pdf))
 * working only with integers (rounds if necessary)
 * reading user input and writing output
 
 
A more precise description of the language and virtual machine for which compiler translates programs can be found in the file [task.pdf](task.pdf). The language and virtual machine were created by Maciej Gębala, who has led this course.

## Technologies:
* Flex - lexical analysis 
* Bison - syntax analysis (parsing)
* C++ - semantic analysis and generating code after parsing by Bison

## How to use compiler?
*Make sure you have installed flex, bison and g++*


**1. Write**

  Write your own program or use one from [examples](examples)

**2. Compile**

  ```
  cd compiler
  make
  compiler yourprogram output
  ```
**3. Run**

  ```
  cd virtual_machine
  make
  maszyna-wirtualna ../compiler/output
  ```
**4. Enjoy**

  Enjoy your program running!
 
