#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
using namespace std;

#include "header.h"

void handleWriteTree(vector<string> commands)
{
    string currDirData = "";
    string currPath = getCurrDir();
    vector<string> fileNames;
    dfs(currPath, fileNames);
    for (int i = 0; i < fileNames.size(); i++)
    {
        string sha;
        if (isDirectory(fileNames[i]))
        {
            sha = calculateFolderSHA1(fileNames[i]);
            currDirData += "040000 tree ";
        }
        else
        {
            sha = calculateFileSHA1(fileNames[i]);
            currDirData += "100644 blob ";
        }

        currDirData += sha;
        currDirData += " ";

        string temp = fileNames[i];
        char *curr = getCurrDir();
        string currDir = curr;

        int n = currDir.size();
        int m = fileNames[i].size();
        string name = fileNames[i].substr(n + 1, m - n);

        currDirData += name;
        currDirData += "\n";
    }

    string stringSha = calculateStringSHA1(INDEX_FILE_PATH);
    string folderName = stringSha.substr(0, 2);
    string folderPath = OBJECTS_DIR + folderName;
    int ffd = createFolder(folderPath);

    string fileName = stringSha.substr(2, 38);
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
            cout << stringSha << "\n";
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

    string tempFile = OBJECTS_DIR + "/temp.txt";
    int nfd = createFile(tempFile);
    if (nfd < 0)
    {
        cout << "Error in creating the temp file.\n";
        return;
    }
    write(nfd, currDirData.c_str(), currDirData.size());

    string metadata = "tree " + to_string(currDirData.size()) + "$";

    const char *inputFile = tempFile.c_str();
    const char *outputFile = binFilePath.c_str();
    compress(inputFile, outputFile, metadata);
    close(ffd);
    close(fd);

    if (unlink(tempFile.c_str()) != 0)
    {
        cout << "Error in deleting the temp.txt file.\n";
        return;
    }

    cout << stringSha << "\n";
}