# squeaky
Squeaky is a new programming languages for making games.
It is an implementation of Scheme (subset of R5RS) with a focus on writing simple and portable multimedia applications.

## Details (also Goals)
In no particular order:
* Written in C99
* Portable between Linux, macOS, and Windows
* Utilizes a stack-based bytecode VM
* Load and display images!
* Load and play sounds!
* Load and redner fonts!
* Builtin multimedia powered by SDL2
* Builtin relational databases powered by SQLite
* High-level and low-level APIs (to be simple or specific as necssary)

## Dependencies
[SDL2](https://www.libsdl.org/index.php) (zlib) - Simple DirectMedia Layer
[SQLite](https://www.sqlite.org/index.html) (public domain) - Small, fast, and full-featured SQL database

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

From macOS:
```
brew install make wget gnu-tar mingw-w64
make -f Makefile.mingw
```

## References
You will likely see references to these throughout the code.
* [CI](https://craftinginterpreters.com/) - "Crafting Interpreters" by Bob Nystrom
* [SICP](https://mitpress.mit.edu/sites/default/files/sicp/index.html) - "Structure and Interpretation of Computer Programs" by Abelson and Sussman
* [BOB](https://github.com/eliben/bobscheme) - The "bobscheme" language implementation by Eli Bendersky
* [BYOL](http://www.buildyourownlisp.com/) - "Build Your Own Lisp" by Daniel Holden
* [SFS](http://peter.michaux.ca/articles/scheme-from-scratch-introduction) - "Scheme from Scratch" by Peter Michaux
* [R5RS](https://schemers.org/Documents/Standards/R5RS/r5rs.pdf) - Revised(5) spec of the Scheme language
