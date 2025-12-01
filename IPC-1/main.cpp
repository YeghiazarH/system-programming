#include <iostream>
#include <csignal>
#include <unistd.h>
#include <pwd.h>
#include <ucontext.h>
#include <string.h>

void write_str(const char* s) {
    write(1, s, strlen(s));
}

void write_num(long v) {
    char buf[64];
    int len = snprintf(buf, sizeof(buf), "%ld", v);
    write(1, buf, len);
}

void handler(int sig, siginfo_t *si, void *context) {
    (void)sig;
    pid_t pid = si ? si->si_pid : -1;
    uid_t uid = si ? si->si_uid : (uid_t)-1;

    const char* username = "(unknown)";
    struct passwd* pw = getpwuid(uid);
    if(pw) username = pw->pw_name;

    ucontext_t* uc = (ucontext_t*)context;
    unsigned long ip = 0, ax = 0, bx = 0;

#if defined(__x86_64__)
    ip = uc->uc_mcontext.gregs[REG_RIP];
    ax = uc->uc_mcontext.gregs[REG_RAX];
    bx = uc->uc_mcontext.gregs[REG_RBX];
#elif defined(__i386__)
    ip = uc->uc_mcontext.gregs[REG_EIP];
    ax = uc->uc_mcontext.gregs[REG_EAX];
    bx = uc->uc_mcontext.gregs[REG_EBX];
#endif

    write_str("Received SIGUSR1 from PID ");
    write_num(pid);
    write_str(" executed by UID ");
    write_num(uid);
    write_str(" (");
    write_str(username);
    write_str(")\n");

    write_str("State of context: IP = ");
    write_num(ip);
    write_str(" AX = ");
    write_num(ax);
    write_str(" BX = ");
    write_num(bx);
    write_str("\n");
}

int main() {
    write_str("PID: ");
    write_num(getpid());
    write_str("\n");

    struct sigaction sa = {};
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    if(sigaction(SIGUSR1, &sa, nullptr) == -1) {
        perror("sigaction");
        return 1;
    }

    while(1) sleep(10);

    return 0;
}
