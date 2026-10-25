# Name

raisetrace - print siginfo and stack trace on signal

# Synopsis

```C
#include <raisetrace.h>

int raisetrace(const int *siglist);
```

# Description

**raisetrace**() installs a signal handler via **sigaction**(2) for each of the signals specified in _siglist_. The _siglist_ array must be zero-terminated. Only signals defined by POSIX.1-2008 are recognized; other signals simply generate an "unknown signal" message instead of a descriptive one.

See [**raisetrace_faults**(3)](raisetrace_faults.3.md) for supplementary predefined lists of signals.

If [**raisetrace_callback**(3)](raisetrace_callback.3.md) is set to any non-NULL value, the installed handler simply invokes it and then returns.

Otherwise if [**raisetrace_callback**(3)](raisetrace_callback.3.md) is NULL, the handler instead prints a message describing the raised signal and a stack trace (when supported by the platform) to the standard error stream, and then performs the default action for the signal.

The installed handlers are not special in any way and may be overriden or reinstated again at any point for any given signal, but note that this function is not thread-safe nor guaranteed to be async-signal-safe, so this is probably best done during program initialization.

# Return value

Returns 0 on success, a negative error number on failure.

# Errors

+ **-EINVAL**

  One or more values in _siglist_ either aren't a valid signal number, or represent a signal that cannot be caught. Rest of signals, if any, will still have been processed correctly.

Note that all error codes are negated. To convert them to regular **errno**(3) values for comparison or passing to **strerror**(3), apply unary negation on the return value first.

# Reporting bugs

If you think you've found a bug, please open an issue in <https://github.com/deimonn/raisetrace/issues> or email <deimonn@outlook.com>.

# See also

**sigaction**(2), [**raisetrace_callback**(3)](raisetrace_callback.3.md), [**raisetrace_faults**(3)](raisetrace_faults.3.md)
