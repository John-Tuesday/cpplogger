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

### Thread safety

There is **no** thread saftey by default, but maybe in the future. At least make thread saftey easy to acheive.

## Usage

### Inject default logger

> advice as of 2026-03-02

Specialize `cpplogger::Defaults<cpplogger::DefaultTag>` such that
`cpplogger::Defaults<cpplogger::DefaultTag>::Logger{}` produces a logger object.
Make sure this specialization is defined before including `cpplogger/log.hpp`.
I recommend essentially replacing `cpplogger/log.hpp` with a header that defines
this specialization, then includes `cpplogger/log.hpp` afterwords.

```cpp
template <>
struct cpplogger::Defaults<cpplogger::DefaultTag> {
  template <typename CharT>
  using Logger = cpplogger::test::AltLogger;
};

#include <cpplogger/log.hpp>
```

