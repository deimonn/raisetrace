#include <raisetrace.h>

#include <backtrace.h>

#include <errno.h>
#include <signal.h>

const int raisetrace_faults[] = {
    SIGABRT, SIGBUS, SIGFPE, SIGILL, SIGQUIT, SIGSEGV, SIGSYS, SIGTRAP, SIGXCPU,
    SIGXFSZ, 0
};

const int raisetrace_unhandled[] = {
    SIGALRM, SIGHUP, SIGINT, SIGPIPE, SIGTERM, SIGUSR1, SIGUSR2, SIGVTALRM, 0
};

void (*raisetrace_callback)(int num, const char *msg,
                            struct backtrace_state *state);

static struct backtrace_state *backtrace_state;

static int print_message(char *buf, int bufsz, int signum, siginfo_t *si)
{
    switch (signum) {
    case SIGABRT:
        return snprintf(buf, bufsz, "raised SIGABRT: process abort signal");
    case SIGALRM:
        return snprintf(buf, bufsz, "raised SIGALRM: alarm clock");
    case SIGBUS:
        switch (si->si_code) {
        case BUS_ADRALN:
            return snprintf(buf, bufsz,
                            "raised SIGBUS: BUS_ADRALN: invalid address alignment for address %p",
                            si->si_addr);
        case BUS_ADRERR:
            return snprintf(buf, bufsz,
                            "raised SIGBUS: BUS_ADRALN: nonexistent physical address %p",
                            si->si_addr);
        case BUS_OBJERR:
            return snprintf(buf, bufsz,
                            "raised SIGBUS: BUS_OBJERR: object-specific hardware error at %p",
                            si->si_addr);
        default:
            return snprintf(buf, bufsz,
                            "raised SIGBUS: access to undefined portion of a memory object at %p",
                            si->si_addr);
        }
    case SIGCHLD:
        switch (si->si_code) {
        case CLD_EXITED:
            return snprintf(buf, bufsz,
                            "raised SIGCHLD: CLD_EXITED: child %i has exited with code %i",
                            si->si_pid, si->si_status);
        case CLD_KILLED:
            return snprintf(buf, bufsz,
                            "raised SIGCHLD: CLD_KILLED: child %i has terminated abnormally without a core dump",
                            si->si_pid);
        case CLD_DUMPED:
            return snprintf(buf, bufsz,
                            "raised SIGCHLD: CLD_DUMPED: child %i has terminated abnormally and created a core dump",
                            si->si_pid);
        case CLD_TRAPPED:
            return snprintf(buf, bufsz,
                            "raised SIGCHLD: CLD_TRAPPED: traced child %i has trapped",
                            si->si_pid);
        case CLD_STOPPED:
            return snprintf(buf, bufsz,
                            "raised SIGCHLD: CLD_STOPPED: child %i has stopped",
                            si->si_pid);
        case CLD_CONTINUED:
            return snprintf(buf, bufsz,
                            "raised SIGCHLD: CLD_CONTINUED: stopped child %i has continued",
                            si->si_pid);
        default:
            return snprintf(buf, bufsz,
                            "raised SIGCHLD: child process %i terminated, stopped or continued",
                            si->si_pid);
        }
    case SIGCONT:
        return snprintf(buf, bufsz, "raised SIGCONT: continue executing");
    case SIGFPE:
        switch (si->si_code) {
        case FPE_INTDIV:
            return snprintf(buf, bufsz,
                            "raised SIGFPE: FPE_INTDIV: integer divide by zero at %p",
                            si->si_addr);
        case FPE_INTOVF:
            return snprintf(buf, bufsz,
                            "raised SIGFPE: FPE_INTOVF: integer overflow at %p",
                            si->si_addr);
        case FPE_FLTDIV:
            return snprintf(buf, bufsz,
                            "raised SIGFPE: FPE_FLTDIV: floating-point divide by zero at %p",
                            si->si_addr);
        case FPE_FLTOVF:
            return snprintf(buf, bufsz,
                            "raised SIGFPE: FPE_FLTOVF: floating-point overflow at %p",
                            si->si_addr);
        case FPE_FLTUND:
            return snprintf(buf, bufsz,
                            "raised SIGFPE: FPE_FLTDIV: floating-point underflow at %p",
                            si->si_addr);
        case FPE_FLTRES:
            return snprintf(buf, bufsz,
                            "raised SIGFPE: FPE_FLTRES: floating-point inexact result at %p",
                            si->si_addr);
        case FPE_FLTINV:
            return snprintf(buf, bufsz,
                            "raised SIGFPE: FPE_FLTINV: invalid floating-point operation at %p",
                            si->si_addr);
        case FPE_FLTSUB:
            return snprintf(buf, bufsz,
                            "raised SIGFPE: FPE_FLTSUB: subscript out of range at %p",
                            si->si_addr);
        default:
            return snprintf(buf, bufsz,
                            "raised SIGFPE: erroneous arithmetic operation at %p",
                            si->si_addr);
        }
    case SIGHUP:
        return snprintf(buf, bufsz, "raised SIGHUP: hangup");
    case SIGILL:
        switch (si->si_code) {
        case ILL_ILLOPC:
            return snprintf(buf, bufsz,
                            "raised SIGILL: ILL_ILLOPC: illegal opcode at %p",
                            si->si_addr);
        case ILL_ILLOPN:
            return snprintf(buf, bufsz,
                            "raised SIGILL: ILL_ILLOPN: illegal operand at %p",
                            si->si_addr);
        case ILL_ILLADR:
            return snprintf(buf, bufsz,
                            "raised SIGILL: ILL_ILLADR: illegal addressing mode at %p",
                            si->si_addr);
        case ILL_ILLTRP:
            return snprintf(buf, bufsz,
                            "raised SIGILL: ILL_ILLTRP: illegal trap at %p",
                            si->si_addr);
        case ILL_PRVOPC:
            return snprintf(buf, bufsz,
                            "raised SIGILL: ILL_PRVOPC: privileged opcode at %p",
                            si->si_addr);
        case ILL_PRVREG:
            return snprintf(buf, bufsz,
                            "raised SIGILL: ILL_PRVREG: privileged register at %p",
                            si->si_addr);
        case ILL_COPROC:
            return snprintf(buf, bufsz,
                            "raised SIGILL: ILL_COPROC: coprocessor error at %p",
                            si->si_addr);
        case ILL_BADSTK:
            return snprintf(buf, bufsz,
                            "raised SIGILL: ILL_BADSTK: internal stack error at %p",
                            si->si_addr);
        default:
            return snprintf(buf, bufsz,
                            "raised SIGILL: illegal instruction at %p",
                            si->si_addr);
        }
    case SIGINT:
        return snprintf(buf, bufsz, "raised SIGINT: terminal interrupt signal");
    case SIGPIPE:
        return snprintf(buf, bufsz,
                        "raised SIGPIPE: write on a pipe with no one to read it");
    case SIGQUIT:
        return snprintf(buf, bufsz, "raised SIGQUIT: terminal quit signal");
    case SIGSEGV:
        switch (si->si_code) {
        case SEGV_MAPERR:
            return snprintf(buf, bufsz,
                            "raised SIGSEGV: SEGV_MAPERR: address %p not mapped to object",
                            si->si_addr);
        case SEGV_ACCERR:
            return snprintf(buf, bufsz,
                            "raised SIGSEGV: SEGV_ACCERR: invalid permissions for mapped object at %p",
                            si->si_addr);
        default:
            return snprintf(buf, bufsz,
                            "raised SIGSEGV: invalid memory reference to address %p",
                            si->si_addr);
        }
    case SIGTERM:
        return snprintf(buf, bufsz, "raised SIGTERM: termination signal");
    case SIGTSTP:
        return snprintf(buf, bufsz, "raised SIGTSTP: terminal stop signal");
    case SIGTTIN:
        return snprintf(buf, bufsz,
                        "raised SIGTTIN: background process attempting read");
    case SIGTTOU:
        return snprintf(buf, bufsz,
                        "raised SIGTTOU: background process attempting write");
    case SIGUSR1:
        return snprintf(buf, bufsz, "raised SIGUSR1: user-defined signal");
    case SIGUSR2:
        return snprintf(buf, bufsz, "raised SIGUSR2: user-defined signal");
#ifdef SIGPOLL
    case SIGPOLL:
        switch (si->si_code) {
        case POLL_IN:
            return snprintf(buf, bufsz,
                            "raised SIGPOLL: POLL_IN: data input available");
        case POLL_OUT:
            return snprintf(buf, bufsz,
                            "raised SIGPOLL: POLL_OUT: output buffers available");
        case POLL_MSG:
            return snprintf(buf, bufsz,
                            "raised SIGPOLL: POLL_MSG: input message available");
        case POLL_ERR:
            return snprintf(buf, bufsz, "raised SIGPOLL: POLL_ERR: I/O error");
        case POLL_PRI:
            return snprintf(buf, bufsz,
                            "raised SIGPOLL: POLL_PRI: high priority input available");
        case POLL_HUP:
            return snprintf(buf, bufsz,
                            "raised SIGPOLL: POLL_HUP: device disconnected");
        default:
            return snprintf(buf, bufsz, "raised SIGPOLL: pollable event");
        }
#endif
#ifdef SIGPROF
    case SIGPROF:
        return snprintf(buf, bufsz, "raised SIGPROF: profiling timer expired");
#endif
#ifdef SIGSYS
    case SIGSYS:
        return snprintf(buf, bufsz, "raised SIGSYS: bad system call");
#endif
    case SIGTRAP:
        switch (si->si_code) {
#ifdef TRAP_BRKPT
        case TRAP_BRKPT:
            return snprintf(buf, bufsz,
                            "raised SIGTRAP: TRAP_BRKPT: process breakpoint");
#endif
#ifdef TRAP_TRACE
        case TRAP_TRACE:
            return snprintf(buf, bufsz,
                            "raised SIGTRAP: TRAP_TRACE: process trace trap");
#endif
        default:
            return snprintf(buf, bufsz,
                            "raised SIGTRAP: trace/breakpoint trap");
        }
    case SIGURG:
        return snprintf(buf, bufsz,
                        "raised SIGURG: high bandwidth data is available at a socket");
#ifdef SIGVTALRM
    case SIGVTALRM:
        return snprintf(buf, bufsz, "raised SIGVTALRM: virtual timer expired");
#endif
    case SIGXCPU:
        return snprintf(buf, bufsz, "raised SIGXCPU: CPU time limit exceeded");
    case SIGXFSZ:
        return snprintf(buf, bufsz, "raised SIGXFSZ: file size limit exceeded");
    default:
        if (signum >= SIGRTMIN && signum <= SIGRTMAX)
            return snprintf(buf, bufsz,
                            "raised SIGRTMIN+%i: user-defined realtime signal",
                            signum - SIGRTMIN);

        return snprintf(buf, bufsz, "raised unknown signal");
    }
}

static void default_action(int signum)
{
    struct sigaction action = {0};
    struct sigaction old_action = {0};

    action.sa_handler = SIG_DFL;

    sigaction(signum, &action, &old_action);
    raise(signum);
    sigaction(signum, &old_action, NULL);
}

static void raisetrace_action(int signum, siginfo_t *siginfo, void *context)
{
    (void)context;

    char message[256];

    print_message(message, sizeof(message) - 1, signum, siginfo);

    if (raisetrace_callback) {
        raisetrace_callback(signum, message, backtrace_state);
        return;
    }

    fprintf(stderr, "%s\n\n", message);

    if (backtrace_state)
        backtrace_print(backtrace_state, 1, stderr);

    default_action(signum);
}

int raisetrace(const int *siglist)
{
    if (!backtrace_state)
        backtrace_state = backtrace_create_state(NULL, 1, NULL, NULL);

    struct sigaction action = {0};

    action.sa_flags = SA_SIGINFO | SA_NODEFER;
    action.sa_sigaction = raisetrace_action;

    int error = 0;

    for (size_t i = 0; siglist[i]; i++) {
        if (sigaction(siglist[i], &action, NULL) != 0)
            error = -EINVAL;
    }

    return error;
}
