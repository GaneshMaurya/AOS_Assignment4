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

    off_t fileSize = lseek(fd, 0, SEEK_END);
    if (fileSize == -1)
    {
        close(fd);
        return;
    }

    char *buffer = new char[fileSize + 1];
    lseek(fd, 0, SEEK_SET);

    ssize_t bytesRead = read(fd, buffer, fileSize);
    if (bytesRead != fileSize)
    {
        delete[] buffer;
        close(fd);
        return;
    }

    buffer[bytesRead] = '\0';
    vector<string> lines;
    string currentLine;
    for (int i = 0; i < bytesRead; i++)
    {
        if (buffer[i] == '\n')
        {
            lines.push_back(currentLine);
            currentLine.clear();
        }
        else
        {
            currentLine += buffer[i];
        }
    }

    if (!currentLine.empty())
    {
        lines.push_back(currentLine);
    }

    for (auto it = lines.begin(); it != lines.end(); it++)
    {
        cout << *it << endl;
    }

    delete[] buffer;
    close(fd);
}