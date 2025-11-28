#include <iostream>
#include <csignal>
#include <unistd.h>
#include <pwd.h>
#include <ucontext.h>


void handler(int sig, siginfo_t *si, void *context) {
    (void)sig;
    pid_t pid = si ? si->si_pid : -1;
    uid_t uid = si ? si->si_uid : (uid_t)-1;

    const char* username = "(unknown)";
    struct passwd* pw = getpwuid(uid);
    if (pw) username = pw->pw_name;

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
    std::cout << "Received SIGUSR1 from PID "
              << pid << " executed by UID "
              << uid << " (" << username << ")" << std::endl;

    std::cout << "State of context: IP = " << ip
              << " AX = " << ax
              << " BX = " << bx
              << std::endl;
}

int main() {
    //usage// kill -SIGUSR1 PID
    std::cout<<"PID: "<<getpid()<<std::endl;

    struct sigaction sa = {};
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, nullptr) == -1) {
        perror("sigaction");
        return 1;
    }

    while (1) sleep(10);

    return 0;
}

