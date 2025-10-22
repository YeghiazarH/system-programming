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

// Trim space from ends
void trim(string& s) {
    s.erase(0, s.find_first_not_of(" \t\n\r"));
    s.erase(s.find_last_not_of(" \t\n\r") + 1);
}

// Execute command and return its exit status
int DoCommand(string cmd, bool silent)
{
    trim(cmd);
    if (cmd.empty()) return 0;

    // Check for output redirection
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

    trim(cmd);
    if (!outFile.empty()) trim(outFile);

    pid_t pid = fork();
    if (pid == -1) {
        cerr << "Error: " << strerror(errno) << endl;
        return -1;
    }

    if (pid == 0) {
        // Child process
        // Add current directory to PATH
        const char* path_env = getenv("PATH");
        string path = path_env ? string(path_env) : "";
        path = "./:" + path;
        setenv("PATH", path.c_str(), 1);

        // Handle output redirection
        if (silent) {
            string outputFile = to_string(getpid()) + ".log";
            int fd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd != -1) {
                dup2(fd, STDOUT_FILENO);
                dup2(fd, STDERR_FILENO);
                close(fd);
            }
        } else if (!outFile.empty()) {
            int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
            int fd = open(outFile.c_str(), flags, 0644);
            if (fd != -1) {
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
        }

        // Parse command into arguments
        vector<char*> args;
        char* cmd_copy = strdup(cmd.c_str());
        char* token = strtok(cmd_copy, " \t");
        while (token != nullptr) {
            args.push_back(token);
            token = strtok(nullptr, " \t");
        }
        args.push_back(nullptr);

        // Execute the command
        execvp(args[0], args.data());

        // If execvp returns, it failed
        cerr << "Command execution failed: " << strerror(errno) << endl;
        free(cmd_copy);
        exit(127);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        return -1;
    }
}

// Token types for parsing
enum TokenType { CMD, AND, OR, SEMI };

struct Token {
    TokenType type;
    string cmd;
};

// Parse command into tokens
vector<Token> tokenize(const string& input) {
    vector<Token> tokens;
    string current;
    size_t i = 0;

    while (i < input.length()) {
        // Check for operators
        if (i + 1 < input.length()) {
            if (input[i] == '&' && input[i+1] == '&') {
                if (!current.empty()) {
                    trim(current);
                    if (!current.empty()) {
                        tokens.push_back({CMD, current});
                    }
                    current.clear();
                }
                tokens.push_back({AND, ""});
                i += 2;
                continue;
            } else if (input[i] == '|' && input[i+1] == '|') {
                if (!current.empty()) {
                    trim(current);
                    if (!current.empty()) {
                        tokens.push_back({CMD, current});
                    }
                    current.clear();
                }
                tokens.push_back({OR, ""});
                i += 2;
                continue;
            }
        }

        if (input[i] == ';') {
            if (!current.empty()) {
                trim(current);
                if (!current.empty()) {
                    tokens.push_back({CMD, current});
                }
                current.clear();
            }
            tokens.push_back({SEMI, ""});
            i++;
            continue;
        }

        current += input[i];
        i++;
    }

    if (!current.empty()) {
        trim(current);
        if (!current.empty()) {
            tokens.push_back({CMD, current});
        }
    }

    return tokens;
}

// Process a command with operators: ;, &&, ||, >, >>
void ProcessCommand(string cmd, bool silent)
{
    trim(cmd);
    if (cmd.empty()) return;

    vector<Token> tokens = tokenize(cmd);
    if (tokens.empty()) return;

    int lastExitCode = 0;
    bool executeNext = true;

    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == CMD) {
            if (executeNext) {
                lastExitCode = DoCommand(tokens[i].cmd, silent);
            }

            // Determine if next command should execute based on next operator
            if (i + 1 < tokens.size()) {
                TokenType nextOp = tokens[i + 1].type;

                if (nextOp == SEMI) {
                    // Always execute next command after ;
                    executeNext = true;
                } else if (nextOp == AND) {
                    // Execute next only if current succeeded
                    executeNext = (lastExitCode == 0);
                } else if (nextOp == OR) {
                    // Execute next only if current failed
                    executeNext = (lastExitCode != 0);
                }

                // Skip the operator token
                i++;
            } else {
                executeNext = true;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    while (1) {
        string cmd;
        getline(cin, cmd);
        
        if (cmd.empty())
            continue;
            
        if (cmd == "exit")
            return 0;
        
        // Check for silent
        bool silent = false;
        if (cmd.find("silent ") == 0) {
            silent = true;
            cmd = cmd.substr(7);
        }
        
        ProcessCommand(cmd, silent);
    }
    
    return 0;
}