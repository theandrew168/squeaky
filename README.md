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
### Booleans
### Pairs and Lists
### Symbols
### Strings
### Control Features
### Eval
### Ports
### Input
### Output
### Windows
### Events

## References
You will likely see references to these throughout the code.
* [CI](https://craftinginterpreters.com/) - "Crafting Interpreters" by Bob Nystrom
* [SICP](https://mitpress.mit.edu/sites/default/files/sicp/index.html) - "Structure and Interpretation of Computer Programs" by Abelson and Sussman
* [BOB](https://github.com/eliben/bobscheme) - The "bobscheme" language implementation by Eli Bendersky
* [BYOL](http://www.buildyourownlisp.com/) - "Build Your Own Lisp" by Daniel Holden
* [SFS](http://peter.michaux.ca/articles/scheme-from-scratch-introduction) - "Scheme from Scratch" by Peter Michaux
* [R5RS](https://schemers.org/Documents/Standards/R5RS/r5rs.pdf) - Revised(5) specification for the Scheme language
