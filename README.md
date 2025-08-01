# CppLogger

Simple logging framework in `C++23`.

This isn't meant to be a *one-size fits all* solution, nor is it meant to the best at doing what does.

## Features 

### Formating

Log message formatting is transparently handled by the standard library, so compile-time checking and any Lsp hints/analysis should work perfectly.
Terminal output can customized independantly to provide colored output.

### Configurable

Most every class and function is implemented using templates, so custom types can be provide custom behavior.
Requirements are checked using concepts.
Base classes and default implementations are provided using compile-time polymorphism.
Default behavior can be changed by specializing the corresponding default templated class.

### Overhead

The actual logging logic is fundamentally just calls to standard print functions, so the overhead is mostly determined by the output stream implementation.

I tried my best to use templated functions that can be disabled such that the compiler will remove them entirely.
Unfortunately, I have not tested this either.

### Thread safety

> I have not tested or verified this aspect...

Syncronization is provided by wrapping output streams using `<syncstream>` from the standard library.
