# raisetrace

**raisetrace** is a simple library for generating messages with stack traces when a signal or exception is raised. You set it up like so:

```C
#include <raisetrace.h>

int main(int argc, char **argv)
{
    raisetrace(raisetrace_faults);

    // ...
}
```

This would make the program generate a stack trace before it terminates when it faults. More information is available on the [**raisetrace**(3)](raisetrace.3.md) man page.

## Building

### Prerequisites

- UNIX-like environment (use [MSYS2](https://www.msys2.org/) on Windows)
- C99 compiler
- Make
- [libbacktrace](https://github.com/ianlancetaylor/libbacktrace) ≥1.0
- (Optional) [md2manc](https://github.com/deimonn/md2manc) ≥0.1
  - Used to generate manpages; pass `DOCS=` to Make to omit

### Procedure

Build with `make`, install with `make install`, run tests with `make check`.

The default configuration is a debug build. To build and install a release configuration, try:

```Shell
make CFLAGS="-O2" install
```

Configuration must be done by overriding values as per above. Environment variables are ignored. All options, their default values, and their explanations can be found at the top of the [Makefile](Makefile).

## Contributing

Contributions are welcome. The code here follows the [Linux kernel coding style](https://kernel.org/doc/html/latest/process/coding-style.html) but with 4-space indentation, disregarding the **Conditional Compilation** section, and disregarding that which is only applicable inside the kernel.

Code format is partially enforced using GNU indent through a script that prints out a diff when changes are necessary. Run it via `make format` (ensure you have `indent` installed).

Beyond that, if you wish to contribute changes to the project, just ensure the default debug configuration builds cleanly and that all tests are passing.

## Future directions

The library is in a usable state but may still be subject to change. Once tests are up, if the library remains stable for long enough, it may get bumped to 1.0.
