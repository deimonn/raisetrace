# Name

raisetrace_callback, RAISETRACE_DEFAULT, RAISETRACE_CONTINUE - function to call when a signal is raised

# Synopsis

```C
#include <raisetrace.h>

#define RAISETRACE_DEFAULT  0
#define RAISETRACE_CONTINUE 1

extern int (*raisetrace_callback)(int num, const char *msg,
                                  struct backtrace_state *state);
```

# Description

**raisetrace_callback** specifies a function to be called by the handler installed by [**raisetrace**(3)](raisetrace.3.md). It is ignored if its NULL, which is its default value.

**raisetrace_callback** receives three arguments:

+ **int** _num_

  Raised signal number or exception code.

+ **const char \***_msg_

  String describing the raised signal or exception, including details when available.

+ **struct backtrace_state \***_state_

  Library's backtrace state, which can be used to generate a stack trace. NULL when unavailable.

**RAISETRACE_CONTINUE** and **RAISETRACE_DEFAULT** are possible return values for the callback:

+ **RAISETRACE_CONTINUE**

  If the callback returns this value, the handler tries to continue execution of the program regardless of the kind of signal or exception raised.

+ **RAISETRACE_DEFAULT**

  If the callback returns this value or any other not specified above, it will perform the default action for the signal or exception, which may mean continuing execution or terminating abnormally.

# Example

The following code illustrates how **raisetrace_callback** can be used to print crashes to a file rather than to standard error:

```C
#include <backtrace.h>
#include <raisetrace.h>

#include <stdlib.h>
#include <stdio.h>

static FILE *crash_log;

static int custom_callback(int num, const char *msg,
                           struct backtrace_state *state)
{
    (void)num;

    fprintf(crash_log, "%s\n\n", msg);

    if (state)
        backtrace_print(state, 1, crash_log);

    return RAISETRACE_DEFAULT;
}

int main(void)
{
    crash_log = fopen("crash.txt", "w");

    if (!crash_log) {
        fprintf(stderr, "failed to open 'crash.txt' for writing");
        return 1;
    }

    raisetrace_callback = custom_callback;
    raisetrace(raisetrace_faults);

    // ...
}
```

# Reporting bugs

If you think you've found a bug, please open an issue in <https://github.com/deimonn/raisetrace/issues> or email <deimonn@outlook.com>.

# See also

**sigaction**(2), [**raisetrace**(3)](raisetrace.3.md)
