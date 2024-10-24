#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

#include "header.h"

const int BUFFER_SIZE = 1024;

void handleHash(vector<string> commands)
{
    // Check if the flag is present or not
    if (commands.size() == 2)
    {
        string fileSha = calculateFileSHA1(commands[1]);
        cout << fileSha << "\n";
    }
    else if (commands.size() == 3)
    {
        if (commands[1] == "-w")
        {
            string fileSha = calculateFileSHA1(commands[2]);
            string folderName = fileSha.substr(0, 2);
            string folderPath = ".mygit/objects/" + folderName;
            int ffd = createFolder(folderPath);
            if (ffd < 0)
            {
                cout << "Error in creating the folder.\n";
                close(ffd);
                return;
            }

            string fileName = fileSha.substr(2, 38);
            string binFilePath = folderPath + "/" + fileName;
            int fd = createFile(binFilePath);
            if (fd < 0)
            {
                cout << "Error in creating the blob file.\n";
                close(fd);
                return;
            }

            string path = commands[2];
            int ufd = open(path.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
            if (ufd < 0)
            {
                cout << "Error in creating the index file.\n";
                close(ufd);
                return;
            }

            vector<char> buffer(BUFFER_SIZE);
            ssize_t bytesRead = read(ufd, buffer.data(), buffer.size());
            if (bytesRead == -1)
            {
                perror("read");
                close(ufd);
                return;
            }

            string temp = "blob " + to_string(bytesRead) + "$";
            lseek(ufd, 0, SEEK_SET);
            ssize_t bytesWritten = write(ufd, temp.c_str(), temp.size());
            if (bytesWritten == -1)
            {
                close(ufd);
                return;
            }

            write(ufd, buffer.data(), bytesRead);

            const char *inputFile = commands[2].c_str();
            const char *outputFile = binFilePath.c_str();
            compress(inputFile, outputFile);
            close(ufd);
            close(fd);
        }
    }
    else
    {
        cout << "Wrong commands entered. Please retry.\n";
    }
}