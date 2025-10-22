#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <cerrno>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <fcntl.h>
#include <sstream>
#include <vector>
using namespace std;

int DoCommand(string cmd, bool silent)
{
   
    bool append = false;
    string outFile;
    size_t pos = cmd.find(">>");
    if (pos != string::npos) {
        append = true;
        outFile = cmd.substr(pos + 2);
        cmd = cmd.substr(0, pos);
    } else if ((pos = cmd.find(">")) != string::npos) {
        outFile = cmd.substr(pos + 1);
        cmd = cmd.substr(0, pos);
    }
    
   
    auto trim = [](string& s) {
        s.erase(0, s.find_first_not_of(" \t"));
        s.erase(s.find_last_not_of(" \t") + 1);
    };
    trim(cmd);
    if (!outFile.empty()) trim(outFile);
    
    pid_t pid = fork();
    if (pid == -1) {
        cout << "Error: " << strerror(errno) << endl;
        return -1;
    }
    if (pid == 0) {
        std::string path = getenv("PATH");
        path = "./:" + path;
        setenv("PATH", path.c_str(), 1);
        
        if (silent) {
            string outputFile = std::to_string(getpid()) + ".log";
            int fd = open(outputFile.c_str(), O_WRONLY | O_CREAT, 0644);
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
        } else if (!outFile.empty()) {
            int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
            int fd = open(outFile.c_str(), flags, 0644);
            dup2(fd, STDOUT_FILENO);
        }
        
        char* args[64];
        char *token = strtok(&cmd[0], " ");
        int i = 0;
        while (token != nullptr) {
            args[i++] = token;
            token = strtok(nullptr, " ");
        }
        args[i] = nullptr;
        execvp(args[0], args);
        std::cerr << "Command execution failed." << std::endl;
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }
}

void ProcessCommand(string& cmd, bool silent)
{
    auto trim = [](string& s) {
        s.erase(0, s.find_first_not_of(" \t"));
        s.erase(s.find_last_not_of(" \t") + 1);
    };

    // Split by || first
    vector<string> orCmds;
    size_t pos = 0;
    size_t found;
    while ((found = cmd.find("||", pos)) != string::npos) {
        string segment = cmd.substr(pos, found - pos);
        trim(segment);
        orCmds.push_back(segment);
        pos = found + 2;
    }
    string lastSeg = cmd.substr(pos);
    trim(lastSeg);
    orCmds.push_back(lastSeg);

    // Process each || segment
    for (auto& orCmd : orCmds) {
        // Split by &&
        vector<string> andCmds;
        pos = 0;
        while ((found = orCmd.find("&&", pos)) != string::npos) {
            string segment = orCmd.substr(pos, found - pos);
            trim(segment);
            andCmds.push_back(segment);
            pos = found + 2;
        }
        lastSeg = orCmd.substr(pos);
        trim(lastSeg);
        andCmds.push_back(lastSeg);

        bool allSuccess = true;
        for (auto& andCmd : andCmds) {
            // Split by ;
            stringstream ss(andCmd);
            string scmd;
            while (getline(ss, scmd, ';')) {
                trim(scmd);
                if (!scmd.empty() && DoCommand(scmd, silent) != 0) {
                    allSuccess = false;
                    break;
                }
            }
            if (!allSuccess) break;
        }

        // If any && chain succeeded, we're done (|| short-circuits)
        if (allSuccess) return;
    }
}

int main(int argc, char* argv[])
{
    while (1) {
        string cmd;
        getline(cin, cmd);
        if(cmd.empty())
            continue;
        if(cmd == "exit")
            return 0;
        bool silent = cmd.find("silent ") == 0;
        if (silent) cmd = cmd.substr(7);
        ProcessCommand(cmd, silent);
    }
    return 0;
}
