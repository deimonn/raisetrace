# raisetrace

**raisetrace** is a simple library for generating messages with stack traces when a signal is received. You set it up like so:

```C
#include <raisetrace.h>

int main(int argc, char **argv)
{
    raisetrace(raisetrace_faults);

    // ...
}
```

This would make the program generate a stack trace before it terminates when it faults. More information is available on the [**raisetrace**(3)](docs/raisetrace.3.md) man page.

## Building

### Prerequisites

- POSIX.1-2008 environment
- C99 compiler
- Meson ≥1.1
- [libbacktrace](https://github.com/ianlancetaylor/libbacktrace) ≥1.0
- (Optional) [md2manc](https://github.com/deimonn/md2manc) ≥0.1

### Procedure

> [!NOTE]
> raisetrace must be built as the same library type that libbacktrace is; if your system's libbacktrace is a static library, pass `-Ddefault_library=static` to `meson setup` or it'll fail to link.

1. Configure and change into the build directory with `meson setup builddir && cd builddir`

2. Build with `ninja`

3. Optionally install the library with `ninja install`

See Meson's [Quickstart Guide](https://mesonbuild.com/Quick-guide.html).

## Contributing

Contributions are welcome. The code here follows the [Linux kernel coding style](https://kernel.org/doc/html/latest/process/coding-style.html) but with 4-space indentation, disregarding the **Conditional Compilation** section, and disregarding that which is only applicable inside the kernel.

Code format is partially enforced using GNU indent by the [check_format.sh](check_format.sh) script, which prints out a diff when changes are necessary. Invoke it like `./check_format.sh` (ensure you have `indent` installed).

Beyond that, if you wish to contribute changes to the project, just ensure it builds cleanly and that all tests are passing.

## Future directions

The library is in a usable state but may still be subject to change. Once tests are up, if the library remains stable for long enough, it may get bumped to 1.0.
