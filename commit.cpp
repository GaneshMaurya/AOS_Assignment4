#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
using namespace std;

#include "header.h"

const string INDEX_FILE_PATH = ".mygit/index.txt";
const string OBJECTS_DIR = ".mygit/objects/";

void handleCommit(vector<string> commands)
{
    string message;

    if (commands.size() == 1)
    {
        // Default message
        message = "Commit";
    }
    else
    {
        string flag = commands[1];

        if (flag != "-m")
        {
            cout << "Please enter correct flag.\n";
            return;
        }
        else
        {
            message = commands[2];
        }
    }

    int ifd = open(INDEX_FILE_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    if (ifd < 0)
    {
        cout << "Error in opening the index file.\n";
        close(ifd);
        return;
    }

    string fileSha = calculateFileSHA1(INDEX_FILE_PATH);
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
            cout << fileSha << "\n";
            // cout << "File already present.\n";
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
        cout << "Error in creating the tree.\n";
        return;
    }

    off_t fileSize = lseek(ifd, 0, SEEK_END);
    lseek(ifd, 0, SEEK_SET);

    string metadata = "commit " + to_string(fileSize) + "$";

    const char *inputFile = INDEX_FILE_PATH.c_str();
    const char *outputFile = binFilePath.c_str();
    compress(inputFile, outputFile, metadata);
    close(ifd);
    close(ffd);
    close(fd);

    cout << fileSha << "\n";
}