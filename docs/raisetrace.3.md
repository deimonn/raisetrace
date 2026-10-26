# Name

raisetrace - print a message and stack trace on signal/exception

# Synopsis

```C
#include <raisetrace.h>

int raisetrace(const int *list);
```

# Description

**raisetrace**() installs a signal or exception handler for each of the signal numbers or exception codes specified in the passed _list_. The _list_ must be zero-terminated.

See [**raisetrace_faults**(3)](raisetrace_faults.3.md) for supplementary predefined lists of signal numbers/exception codes.

If [**raisetrace_callback**(3)](raisetrace_callback.3.md) is set to any non-NULL value, the installed handler simply invokes it and performs the action specified by its return value.

Otherwise if [**raisetrace_callback**(3)](raisetrace_callback.3.md) is NULL, the handler instead prints a message describing the raised signal or exception and a stack trace (when supported by the platform) to the standard error stream, and then performs the default action for said signal or exception.

**raisetrace**() is not thread-safe nor guaranteed to be async-signal-safe, so this function is probably best called during program initialization.

## UNIX-specific behavior

Only signals defined by POSIX.1-2008 are recognized; other signals can still be caught, but they generate a generic "unknown signal" message instead of a descriptive one.

The installed signal handler is not special in any way and may be overriden or reinstated again at any point for any given signal. **raisetrace**() installs signal handlers via **sigaction**(2).

## Windows-specific behavior

Each integer in the _list_ is expected to be an exception code; see <https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-exception_record>.

The installed exception handler is a vectored exception handler, and the mask of exceptions to catch is a **raisetrace**() implementation detail.

# Return value

Returns 0 on success, a negative error number on failure.

# Errors

+ **-EINVAL**

  One or more values in the _list_ either aren't a valid signal number/exception code, or represent a signal or exception that cannot be caught. All other signals/exceptions will still have been processed correctly.

Note that all error codes are negated. To convert them to regular **errno**(3) values for comparison or passing to **strerror**(3), apply unary negation on the return value first.

# Reporting bugs

If you think you've found a bug, please open an issue in <https://github.com/deimonn/raisetrace/issues> or email <deimonn@outlook.com>.

# See also

**sigaction**(2), [**raisetrace_callback**(3)](raisetrace_callback.3.md), [**raisetrace_faults**(3)](raisetrace_faults.3.md)
