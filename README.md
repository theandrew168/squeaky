# squeaky
Squeaky is a proof of concept programming languages for making games.
It is an implementation of Scheme (subset of R5RS) with a focus on writing simple and portable multimedia applications.

## Details
In no particular order:
* Written in C99
* Portable between Linux, macOS, and Windows
* Simple line-based graphics
* Basic keyboard / mouse input
* Powered by SDL2

## Dependencies
[SDL2](https://www.libsdl.org/index.php) (zlib) - Simple DirectMedia Layer  

## Building
This project is built using POSIX-compatible [make](https://pubs.opengroup.org/onlinepubs/009695399/utilities/make.html).
For unix-like systems, it can be built natively.
For Windows builds, [mingw-w64](http://mingw-w64.org/doku.php) is used to cross-compile the project from a unix-like system.

### Linux (Native, Debian-based)
```
sudo apt install make libsdl2-dev
make
```

### macOS (Native)
```
brew install make sdl2
make -f Makefile.macos
```

### Windows (Cross-Compile)
From Linux (Debian-based):
```
sudo apt install make wget tar mingw-w64
make -f Makefile.mingw
```

## Special Forms
**(quote foo)** - Quote the expression 'foo'  
**'foo** - Quote the expression 'foo'  
**(define x 42)** - Define a value in the current environment  
**(define square (lambda (x) (\* x x))** - Define a lambda  
**(define (square x) (\* x x))** - Define a lambda  
**(set! x 24)** - Update an existing value in the current environment  
**(if a b c)** - Conditional operator: if 'a' is true then 'b', else 'c'  
**(interaction-environment)** - Return the current environment  
**(load "foo.scm")** - Load a scheme source file into the current environment  
**(gc)** - Run the garbage collector to free unused memory  

## Procedures
This section describes the subset of R5RS that Squeaky supports.
It also details the builtin multimedia extensions for creating windows and handling events.

### Equivalence Predicates
**(eq? a b)** - Compare two scheme values for equality  
**(eqv? a b)** - Compare two scheme values for equality  
**(equal? a b)** - Compare two scheme values for equality  

### Numerical Operations
**(number? x)** - Check if 'x' is a number  
**(= a b ...)** - Check if all given numbers are equal  
**(< a b ...)** - Check if all given numbers are increasing  
**(> a b ...)** - Check if all given numbers are decreasing  
**(<= a b ...)** - Check if all given numbers are non-decreasing  
**(>= a b ...)** - Check if all given numbers are non-increasing  
**(+ a b ...)** - Successively add all of the given numbers  
**(\* a b ...)** - Successively mulitply all of the given numbers  
**(- a b ...)** - Successively subtract all of the given numbers  
**(/ a b ...)** - Successively divide all of the given numbers  

### Booleans
**(boolean? x)** - Check if 'x' is a boolean  

### Pairs and Lists
**(pair? x)** - Check if 'x' is a pair  
**(cons a b)** - Join 'a' and 'b' together in a pair  
**(car p)** - Return the first element of pair 'p'  
**(cdr p)** - Return the second element of pair 'p'  
**(set-car! p x)** - Update the first element of 'p' to 'x'  
**(set-cdr! p x)** - Update the second element of 'p' to 'x'  
**(null? l)** - Check if 'x' is an empty list  

### Symbols
**(symbol? x)** - Check if 'x' is a symbol  

### Strings
**(string? x)** - Check if 'x' is a string  

### Control Features
**(procedure? x)** - Check if 'x' is a procedure  
**(apply op args ...)** - Apply procedure 'op' to the given args  

### Eval
**(eval exp env)** - Evaluate the expression 'exp' in environment 'env'  

### Ports
**(input-port? x)** - Check if 'x' is an input port  
**(output-port? x)** - Check if 'x' is an output port  
**(current-input-port)** - Return the current input port (stdin)  
**(current-output-port)** - Return the current output port (stdout)  
**(open-input-file path)** - Open file 'path' for reading  
**(open-output-file path)** - Open file 'path' for writing  
**(close-input-port port)** - Close the input port 'port'  
**(close-output-port port)** - Close the output port 'port'  

### Input
**(read [port])** - Read an expression from 'port' (defaults to stdin)  
**(read-char [port])** - Read a character from 'port' (defaults to stdin)  
**(peek-char [port])** - Peek a character from 'port' (defaults to stdin)  
**(eof-object? x)** - Check if 'x' is an EOF object (end of file)  
**(char-ready? [port])** - Check if a character is ready on 'port' (defaults to stdin)  

### Output
**(write exp [port])** - Write an expresstion to 'port' (defaults to stdout)  
**(display exp [port])** - Print an expression to 'port' (defaults to stdout)  
**(newline [port])** - Print a newline to 'port' (defaults to stdout)  
**(write-char char [port])** - Write a character to 'port' (defaults to stdout)

### Windows
**(window? x)** - Check if 'x' is a window  
**(make-window title width height)** - Create a window with the given parameters  
**(window-clear! w)** - Clear the contents of a window  
**(window-draw-line! w x0 y0 x1 y1)** - Draw a line from point 0 to point 1  
**(window-present! w)** - Present the window's current contents  

### Events
**(event? x)** - Check if 'x' is an event  
**(event-poll w)** - Grab the next event on window 'w'  
**(event-type e)** - Return the type of event 'e' (keyboard, quit, etc)  
**(event-key e)** - Return the key from a keyboard event (left, right, escape, etc)  

## References
You will likely see references to these throughout the code.
* [CI](https://craftinginterpreters.com/) - "Crafting Interpreters" by Bob Nystrom
* [SICP](https://mitpress.mit.edu/sites/default/files/sicp/index.html) - "Structure and Interpretation of Computer Programs" by Abelson and Sussman
* [BOB](https://github.com/eliben/bobscheme) - The "bobscheme" language implementation by Eli Bendersky
* [BYOL](http://www.buildyourownlisp.com/) - "Build Your Own Lisp" by Daniel Holden
* [SFS](http://peter.michaux.ca/articles/scheme-from-scratch-introduction) - "Scheme from Scratch" by Peter Michaux
* [R5RS](https://schemers.org/Documents/Standards/R5RS/r5rs.pdf) - Revised(5) specification for the Scheme language
