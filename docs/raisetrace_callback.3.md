# Name

raisetrace_callback - function to call when a signal is raised

# Synopsis

```C
#include <raisetrace.h>

extern void (*raisetrace_callback)(int num, const char *msg,
                                   struct backtrace_state *state);
```

# Description

**raisetrace_callback** specifies a function to be called by the handler installed by [**raisetrace**(3)](raisetrace.3.md). It is ignored if its NULL, which is its default value.

**raisetrace_callback** receives three arguments:

+ **int** _num_

  Number of the raised signal.

+ **const char \***_msg_

  String describing the raised signal, including details when available.

+ **struct backtrace_state \***_state_

  Library's backtrace state, which can be used to generate a stack trace. NULL when unavailable.

If the callback returns, it is equivalent to the signal handler returning, and thus the program will try to continue execution. It is the callee's responsibility to terminate the thread or program in the case of non-continuable signals.

# Example

The following code illustrates how **raisetrace_callback** can be used to print crashes to a file rather than to standard error:

```C
#include <backtrace.h>
#include <raisetrace.h>

#include <stdlib.h>
#include <stdio.h>

static FILE *crash_log;

static void custom_callback(int num, const char *msg,
                            struct backtrace_state *state)
{
    (void)num;

    fprintf(crash_log, "%s\n\n", msg);

    if (state)
        backtrace_print(state, 1, crash_log);

    exit(1);
}

int main(void)
{
    crash_log = fopen("crash.txt", "w");

    if (!crash_log) {
        fprintf(stderr, "failed to open 'crash.txt' for writing");
        return 1;
    }

    raisetrace_callback = custom_callback;

    // ...
}
```

# Reporting bugs

If you think you've found a bug, please open an issue in <https://github.com/deimonn/raisetrace/issues> or email <deimonn@outlook.com>.

# See also

**sigaction**(2), [**raisetrace**(3)](raisetrace.3.md)
