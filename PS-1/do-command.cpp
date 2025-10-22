#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#include <ctime>
#include <cstring>

using namespace std;

void do_command(char **argv) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return;
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    if (pid == 0) {
        execvp(argv[0], argv);
        perror("execvp failed");
        _exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        gettimeofday(&end, NULL);

        double duration = (end.tv_sec - start.tv_sec) +
                          (end.tv_usec - start.tv_usec) / 1000000.0;

        if (WIFEXITED(status))
            cout << "Command completed with " << WEXITSTATUS(status)
                 << " exit code and used " << duration << " seconds." << endl;
        else
            cout << "Command failed or terminated abnormally (signal "
                 << WTERMSIG(status) << ") and used " << duration << " seconds." << endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Please use right input" << endl;
        return 1;
    }

    char **cmd_argv = new char*[argc];
    for (int i = 1; i < argc; i++)
        cmd_argv[i - 1] = argv[i];
    cmd_argv[argc - 1] = NULL;

    do_command(cmd_argv);

    delete[] cmd_argv;
    return 0;
}

