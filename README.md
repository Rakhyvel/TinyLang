# TinyLang
TinyLang is a small, barebones, interpreted programming language developed to demonstrate the basics of writing a compiler or interpreter.
## Tutorial
The tutorial can be found [here](josephs-projects/blog/tinylang1.html)
## Specification
### Variables
Variables can be assigned using the ":" operator. For example:
```
x: 4
```
Here, the variable "x" is assigned the value 4. There are no types in TinyLang, and all variables are 32-bit integers.

User input can be read and assigned to a variable using the postfix "?" operator. For example:
```
x?
```
The program will wait for user input to be given, and will assign the integer value to the given varaible.
### Expressions
TinyLang can parse and evaluate expressions, with a list of some operators.

### Control flow