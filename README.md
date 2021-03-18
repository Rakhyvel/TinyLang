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
TinyLang can parse and evaluate expressions, with a list of some operators. They are listed below, in reverse order of operations.
| Token          | Description                                                                                    |
|----------------|------------------------------------------------------------------------------------------------|
| <*identifier*> | Any text in the program that is not a keyword, or surrounded by quotes. Represents a variable. |
| <*number*>     | Any positive, whole number. Represents a constant value                                        |
| <*string*>     | Any text in the program that is surrounded by double quotes.                                   |
| !              | Postfix operator. prints expression to standard out, returns expression result.                |
| ?              | Postfix operator. Assigns user input to operand, returns expression result.                    |
| :              | Assignment. Assigns expression evaluation to variable, returns expression result.              |
| and            | Logical and.                                                                                   |
| or             | Logical or.                                                                                    |
| =              | Equality comparison.                                                                           |
| >              | Greater-than comparison.                                                                       |
| <              | Less-than comparison.                                                                          |
| +              | Addition.                                                                                      |
| -              | Subtraction.                                                                                   |
| *              | Multiplication.                                                                                |
| /              | Integer division.                                                                              |
| ( *and* )      | Parenthesis, used to explicitly state order of operations.                                     |

Identifiers must start with a letter as their first character, but then can contain digits and underscores.

Number literals must be positive. Negative numbers can be achieved like so:
```
x: 0-4
"x now has the value -4"
```

Strings function as both pieces of text that can be printed out, and as comments, if they are not printed. 
```
"This is a comment."
"This is a piece of text."!
```

### Control flow
There are a few control flow structures defined in TinyLang. They are as follows:
| Token                     | Description                                                   |
|---------------------------|---------------------------------------------------------------|
| { *code* }                | The inside code is executed, counts as one statement.         |
| if *expression* *code*    | If the expression evaluates to true, the code is executed.    |
| while *expression* *code* | While the expression evaluates to true, the code is executed. |
| halt                      | Stops the program.                                            |