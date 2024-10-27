#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
using namespace std;

#include "header.h"

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
            string folderPath = OBJECTS_DIR + folderName;
            int ffd = createFolder(folderPath);
            if (ffd < 0 && errno != EEXIST)
            {
                cout << "Error in creating the folder.\n";
                return;
            }

            string fileName = fileSha.substr(2, 38);
            string binFilePath = folderPath + "/" + fileName;

            // Check if file is already present
            DIR *directory = opendir(folderPath.c_str());
            if (directory == NULL)
            {
                close(ffd);
                return;
            }

            struct dirent *dirInfo;
            bool fileExists = false;
            while ((dirInfo = readdir(directory)) != NULL)
            {
                if (dirInfo->d_name[0] != '.' && fileName == dirInfo->d_name)
                {
                    cout << "File already present.\n";
                    fileExists = true;
                    break;
                }
            }
            closedir(directory);

            if (fileExists)
            {
                close(ffd);
                return;
            }

            int fd = createFile(binFilePath);
            if (fd < 0)
            {
                cout << "Error in creating the blob file.\n";
                return;
            }

            string path = commands[2];
            int ufd = open(path.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
            if (ufd < 0)
            {
                cout << "Error in opening the input file.\n";
                close(fd);
                return;
            }

            off_t fileSize = lseek(ufd, 0, SEEK_END);
            lseek(ufd, 0, SEEK_SET);

            string metadata = "blob " + to_string(fileSize) + "$";

            const char *inputFile = commands[2].c_str();
            const char *outputFile = binFilePath.c_str();
            compress(inputFile, outputFile, metadata);
            close(ufd);
            close(fd);
        }
        else
        {
            cout << "Wrong flag entered.\n";
            return;
        }
    }
    else
    {
        cout << "Wrong commands entered. Please retry.\n";
    }
}