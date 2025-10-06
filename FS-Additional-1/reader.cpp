#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <vector>
#include <string>

using namespace std;

#define BUF_SIZE 4096
#define MAX_LINES 500000

bool read_line(int fd, string &line) {
    static char buf[BUF_SIZE];
    static int pos = 0, size = 0;

    line.clear();
    while (true) {
        if (pos >= size) {
            size = read(fd, buf, BUF_SIZE);
            if (size <= 0) return !line.empty();
            pos = 0;
        }
        if (buf[pos] == '\n') {
            pos++;
            return true;
        }
        line += buf[pos++];
    }
}

void write_line(int fd, const string &line) {
    write(fd, line.c_str(), line.size());
    write(fd, "\n", 1);
}

int split_sort(const char *input) {
    int fd = open(input, O_RDONLY);
    int chunk = 0;

    while (true) {
        vector<string> lines;
        string line;

        while (lines.size() < MAX_LINES && read_line(fd, line)) {
            lines.push_back(line);
        }

        if (lines.empty()) break;

        sort(lines.begin(), lines.end());

        string name = "tmp_" + to_string(chunk++);
        int out = open(name.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);

        for (auto &s : lines) {
            write_line(out, s);
        }
        close(out);
    }

    close(fd);
    return chunk;
}

void copy_file(const char *from, const char *to) {
    int in = open(from, O_RDONLY);
    int out = open(to, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    char buf[BUF_SIZE];
    ssize_t n;
    while ((n = read(in, buf, BUF_SIZE)) > 0) {
        write(out, buf, n);
    }
    close(in);
    close(out);
}

void merge(const char *f1, const char *f2, const char *out_name) {
    int fd1 = open(f1, O_RDONLY);
    int fd2 = open(f2, O_RDONLY);
    int out = open(out_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    string line1, line2;
    bool has1 = read_line(fd1, line1);
    bool has2 = read_line(fd2, line2);

    while (has1 && has2) {
        if (line1 <= line2) {
            write_line(out, line1);
            has1 = read_line(fd1, line1);
        } else {
            write_line(out, line2);
            has2 = read_line(fd2, line2);
        }
    }

    while (has1) {
        write_line(out, line1);
        has1 = read_line(fd1, line1);
    }

    while (has2) {
        write_line(out, line2);
        has2 = read_line(fd2, line2);
    }

    close(fd1);
    close(fd2);
    close(out);
}

int main(int argc, char *argv[]) {
    if (argc < 3) return 1;

    const char *input = argv[1];
    const char *output = argv[2];

    int n = split_sort(input);
    if (n == 0) return 0;

    while (n > 1) {
        int new_n = 0;
        for (int i = 0; i < n; i += 2) {
            string f1 = "tmp_" + to_string(i);
            string out = "mrg_" + to_string(new_n);

            if (i + 1 < n) {
                string f2 = "tmp_" + to_string(i + 1);
                merge(f1.c_str(), f2.c_str(), out.c_str());
                unlink(f1.c_str());
                unlink(f2.c_str());
            } else {
                copy_file(f1.c_str(), out.c_str());
                unlink(f1.c_str());
            }
            new_n++;
        }

        for (int i = 0; i < new_n; i++) {
            string old_name = "mrg_" + to_string(i);
            string new_name = "tmp_" + to_string(i);
            copy_file(old_name.c_str(), new_name.c_str());
            unlink(old_name.c_str());
        }
        n = new_n;
    }

    copy_file("tmp_0", output);
    unlink("tmp_0");
    return 0;
}