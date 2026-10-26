#include <raisetrace.h>

#include <backtrace.h>

#include <errno.h>
#include <stdbool.h>
#include <windows.h>

const int raisetrace_faults[] = {
    EXCEPTION_ACCESS_VIOLATION, EXCEPTION_ARRAY_BOUNDS_EXCEEDED,
    EXCEPTION_BREAKPOINT, EXCEPTION_DATATYPE_MISALIGNMENT,
    EXCEPTION_FLT_DENORMAL_OPERAND, EXCEPTION_FLT_DIVIDE_BY_ZERO,
    EXCEPTION_FLT_INEXACT_RESULT, EXCEPTION_FLT_INVALID_OPERATION,
    EXCEPTION_FLT_OVERFLOW, EXCEPTION_FLT_STACK_CHECK, EXCEPTION_FLT_UNDERFLOW,
    EXCEPTION_GUARD_PAGE, EXCEPTION_ILLEGAL_INSTRUCTION,
    EXCEPTION_IN_PAGE_ERROR, EXCEPTION_INT_DIVIDE_BY_ZERO,
    EXCEPTION_INT_OVERFLOW, EXCEPTION_INVALID_DISPOSITION,
    EXCEPTION_INVALID_HANDLE, EXCEPTION_NONCONTINUABLE_EXCEPTION,
    EXCEPTION_PRIV_INSTRUCTION, EXCEPTION_SINGLE_STEP,
    EXCEPTION_STACK_OVERFLOW, 0
};

const int raisetrace_unhandled[] = {0};

int (*raisetrace_callback)(int num, const char *msg,
                           struct backtrace_state *state);

#define EXCEPTION_INVALID 0
#define EXCEPTION_IGNORE  1
#define EXCEPTION_CATCH   2

static int exception_mask[256] = {
    [EXCEPTION_ACCESS_VIOLATION & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_ARRAY_BOUNDS_EXCEEDED & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_BREAKPOINT & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_DATATYPE_MISALIGNMENT & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_FLT_DENORMAL_OPERAND & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_FLT_DIVIDE_BY_ZERO & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_FLT_INEXACT_RESULT & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_FLT_INVALID_OPERATION & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_FLT_OVERFLOW & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_FLT_STACK_CHECK & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_FLT_UNDERFLOW & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_GUARD_PAGE & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_ILLEGAL_INSTRUCTION & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_IN_PAGE_ERROR & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_INT_DIVIDE_BY_ZERO & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_INT_OVERFLOW & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_INVALID_DISPOSITION & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_INVALID_HANDLE & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_NONCONTINUABLE_EXCEPTION & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_PRIV_INSTRUCTION & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_SINGLE_STEP & 0xFF] = EXCEPTION_IGNORE,
    [EXCEPTION_STACK_OVERFLOW & 0xFF] = EXCEPTION_IGNORE
};

static struct backtrace_state *backtrace_state;

static int print_message(char *buf, int bufsz, PEXCEPTION_RECORD er)
{
    switch (er->ExceptionCode) {
    case EXCEPTION_ACCESS_VIOLATION:
        switch (er->ExceptionInformation[0]) {
        case 0:
            return snprintf(buf, bufsz,
                            "raised EXCEPTION_ACCESS_VIOLATION: thread tried to read from virtual address %p for which it does not have the appropiate access",
                            (void *)er->ExceptionInformation[1]);
        case 1:
            return snprintf(buf, bufsz,
                            "raised EXCEPTION_ACCESS_VIOLATION: thread tried to write to virtual address %p for which it does not have the appropiate access",
                            (void *)er->ExceptionInformation[1]);
        case 8:
            return snprintf(buf, bufsz,
                            "raised EXCEPTION_ACCESS_VIOLATION: thread tried to execute code at virtual address %p for which it does not have the appropiate access",
                            (void *)er->ExceptionInformation[1]);
        default:
            return snprintf(buf, bufsz,
                            "raised EXCEPTION_ACCESS_VIOLATION: thread tried to read from or write to virtual address %p for which it does not have the appropiate access",
                            (void *)er->ExceptionInformation[1]);
        }
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_ARRAY_BOUNDS_EXCEEDED: thread tried to access an array element that is out of bounds");
    case EXCEPTION_BREAKPOINT:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_BREAKPOINT: a breakpoint was encountered");
    case EXCEPTION_DATATYPE_MISALIGNMENT:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_DATATYPE_MISALIGNMENT: thread tried to read or write data that is misaligned");
    case EXCEPTION_FLT_DENORMAL_OPERAND:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_FLT_DENORMAL_OPERAND: one of the operands in a floating-point operation is denormal");
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_FLT_DIVIDE_BY_ZERO: thread tried to divide a floating-point value by zero");
    case EXCEPTION_FLT_INEXACT_RESULT:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_FLT_INEXACT_RESULT: result of floating-point operation cannot be represented exactly as a decimal fraction");
    case EXCEPTION_FLT_INVALID_OPERATION:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_FLT_INVALID_OPERATION: invalid floating-point operation");
    case EXCEPTION_FLT_OVERFLOW:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_FLT_OVERFLOW: exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type");
    case EXCEPTION_FLT_STACK_CHECK:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_FLT_STACK_CHECK: stack overflow or underflowed as the result of a floating-point operation");
    case EXCEPTION_FLT_UNDERFLOW:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_FLT_UNDERFLOW: exponent of a floating-point operation is less than the magnitude allowed by the corresponding type");
    case EXCEPTION_GUARD_PAGE:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_GUARD_PAGE: thread accessed memory allocated with the PAGE_GUARD modifier");
    case EXCEPTION_ILLEGAL_INSTRUCTION:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_ILLEGAL_INSTRUCTION: thread tried to execute an invalid instruction");
    case EXCEPTION_IN_PAGE_ERROR:
        switch (er->ExceptionInformation[0]) {
        case 0:
            return snprintf(buf, bufsz,
                            "raised EXCEPTION_IN_PAGE_ERROR: thread tried to read from a page that was not present at virtual address %p, and the system was unable to load the page",
                            (void *)er->ExceptionInformation[1]);
        case 1:
            return snprintf(buf, bufsz,
                            "raised EXCEPTION_IN_PAGE_ERROR: thread tried to write to a page that was not present at virtual address %p, and the system was unable to load the page",
                            (void *)er->ExceptionInformation[1]);
        case 8:
            return snprintf(buf, bufsz,
                            "raised EXCEPTION_IN_PAGE_ERROR: thread tried to execute code from a page that was not present at virtual address %p, and the system was unable to load the page",
                            (void *)er->ExceptionInformation[1]);
        default:
            return snprintf(buf, bufsz,
                            "raised EXCEPTION_IN_PAGE_ERROR: thread tried to access a page that was not present at virtual address %p, and the system was unable to load the page",
                            (void *)er->ExceptionInformation[1]);
        }
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_INT_DIVIDE_BY_ZERO: thread tried to divide an integer value by zero");
    case EXCEPTION_INT_OVERFLOW:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_INT_DIVIDE_BY_ZERO: result of an integer operation caused a carry out of the most significant bit of the result");
    case EXCEPTION_INVALID_DISPOSITION:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_INVALID_DISPOSITION: exception handler returned an invalid disposition to the exception dispatcher");
    case EXCEPTION_INVALID_HANDLE:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_INVALID_HANDLE: thread used a handle to a kernel object that was invalid");
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_NONCONTINUABLE_EXCEPTION: thread tried to continue execution after a noncontinuable exception occurred");
    case EXCEPTION_PRIV_INSTRUCTION:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_PRIV_INSTRUCTION: thread tried to execute an instruction whose operation is not allowed in the current machine mode");
    case EXCEPTION_SINGLE_STEP:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_SINGLE_STEP: trace trap or other single-instruction mechanism signaled that one instruction has been executed");
    case EXCEPTION_STACK_OVERFLOW:
        return snprintf(buf, bufsz,
                        "raised EXCEPTION_STACK_OVERFLOW: thread used up its stack");
    default:
        return snprintf(buf, bufsz, "raised unknown exception");
    }
}

static LONG raisetrace_handler(PEXCEPTION_POINTERS ep)
{
    PEXCEPTION_RECORD er = ep->ExceptionRecord;

    if (exception_mask[er->ExceptionCode & 0xFF] != EXCEPTION_CATCH)
        return EXCEPTION_CONTINUE_SEARCH;

    char message[256];

    print_message(message, sizeof(message) - 1, er);

    if (raisetrace_callback) {
        int action = raisetrace_callback(er->ExceptionCode, message,
                                         backtrace_state);

        if (action == RAISETRACE_CONTINUE)
            return EXCEPTION_CONTINUE_EXECUTION;

        return EXCEPTION_CONTINUE_SEARCH;
    }

    fprintf(stderr, "%s\n\n", message);

    if (backtrace_state) {
        backtrace_print(backtrace_state, 1, stderr);
        fputc('\n', stderr);
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

int raisetrace(const int *list)
{
    if (!backtrace_state)
        backtrace_state = backtrace_create_state(NULL, 1, NULL, NULL);

    int error = 0;

    for (size_t i = 0; list[i]; i++) {
        if (exception_mask[list[i] & 0xFF] == EXCEPTION_INVALID) {
            error = -EINVAL;
            continue;
        }

        exception_mask[list[i] & 0xFF] = EXCEPTION_CATCH;
    }

    static bool handlerinit = false;

    if (!handlerinit) {
        AddVectoredExceptionHandler(TRUE, raisetrace_handler);
        handlerinit = true;
    }

    return error;
}
