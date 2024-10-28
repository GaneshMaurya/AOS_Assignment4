#include <bits/stdc++.h>
#include <dirent.h>
using namespace std;

#include "header.h"

void handleCheckout(vector<string> commands)
{
    string commitSha = commands[1];
    string folderName = commitSha.substr(0, 2);
    string folderPath = OBJECTS_DIR + folderName;

    string fileName = commitSha.substr(2, 38);
    string binFilePath = folderPath + "/" + fileName;
    int ffd = createFolder(folderPath);
    if (ffd < 0 && errno != EEXIST)
    {
        cout << "Error in creating the folder.\n";
        return;
    }

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
            fileExists = true;
            break;
        }
    }
    closedir(directory);

    if (!fileExists)
    {
        cout << "Hash does not exist.\n";
        return;
    }
}