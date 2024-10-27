#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <chrono>
using namespace std;

#include "header.h"

void handleLog(vector<string> commands)
{
    int fd = open(LOG_HEAD.c_str(), O_RDONLY);
    if (fd < 0)
    {
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
    {
        if (write(STDOUT_FILENO, buffer, bytesRead) != bytesRead)
        {
            close(fd);
            return;
        }
    }

    if (bytesRead < 0)
    {
        return;
    }

    close(fd);
}