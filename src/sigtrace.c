#include <sigtrace.h>

#include <backtrace.h>

#include <errno.h>
#include <signal.h>

const int sigtrace_faults[] = {
    SIGABRT, SIGBUS, SIGFPE, SIGILL, SIGQUIT, SIGSEGV, SIGSYS, SIGTRAP, SIGXCPU,
    SIGXFSZ, 0
};

const int sigtrace_unhandled[] = {
    SIGALRM, SIGHUP, SIGINT, SIGPIPE, SIGTERM, SIGUSR1, SIGUSR2, SIGVTALRM, 0
};

static struct backtrace_state *backtrace_state;

static void default_action(int signum)
{
    struct sigaction action = {0};
    struct sigaction old_action = {0};

    action.sa_handler = SIG_DFL;

    sigaction(signum, &action, &old_action);
    raise(signum);
    sigaction(signum, &old_action, NULL);
}

static void sigtrace_action(int signum, siginfo_t *siginfo, void *context)
{
    (void)context;

    psiginfo(siginfo, NULL);
    fputc('\n', stderr);

    if (backtrace_state)
        backtrace_print(backtrace_state, 1, stderr);

    default_action(signum);
}

int sigtrace(const int *siglist)
{
    if (!backtrace_state)
        backtrace_state = backtrace_create_state(NULL, 1, NULL, NULL);

    struct sigaction action = {0};

    action.sa_flags = SA_SIGINFO | SA_NODEFER;
    action.sa_sigaction = sigtrace_action;

    int error = 0;

    for (size_t i = 0; siglist[i]; i++) {
        if (sigaction(siglist[i], &action, NULL) != 0)
            error = -EINVAL;
    }

    return error;
}
