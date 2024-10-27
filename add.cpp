#include <bits/stdc++.h>
#include <fcntl.h>
#include <dirent.h>
using namespace std;

#include "header.h"

const int BUFFER_SIZE = 8 * 1024;
const string INDEX_FILE_PATH = ".mygit/index.txt";

void dfs(string folderName, vector<string> &files)
{
    DIR *directory = opendir(folderName.c_str());
    if (directory == NULL)
    {
        return;
    }

    struct dirent *dirInfo = readdir(directory);
    while (dirInfo != NULL)
    {
        string path = folderName + "/" + dirInfo->d_name;
        if (dirInfo->d_name[0] != '.')
        {
            if (isDirectory(path))
            {
                files.push_back(path);
                dfs(path, files);
            }
            else
            {
                files.push_back(path);
            }
        }

        dirInfo = readdir(directory);
    }
    closedir(directory);
    return;
}

char *getCurrDir()
{
    char *currDirectory = (char *)malloc(BUFFER_SIZE);
    if (getcwd(currDirectory, 1024) == NULL)
    {
        printf("Error in retrieving current directory");
    }

    return currDirectory;
}

void handleAdd(vector<string> commands)
{
    vector<string> fileNames;
    if (commands[1] == ".")
    {
        string currPath = getCurrDir();
        dfs(currPath, fileNames);

        vector<string> fileShas;
        string textIndexFile = "";
        for (int i = 0; i < fileNames.size(); i++)
        {
            string sha;
            if (isDirectory(fileNames[i]))
            {
                sha = calculateFolderSHA1(fileNames[i]);
                textIndexFile += "040000 tree ";
            }
            else
            {
                sha = calculateFileSHA1(fileNames[i]);
                textIndexFile += "100644 blob ";
            }

            fileShas.push_back(sha);

            textIndexFile += sha;
            textIndexFile += " ";

            string temp = fileNames[i];
            char *curr = getCurrDir();
            string currDir = curr;

            int n = currDir.size();
            int m = fileNames[i].size();
            string name = fileNames[i].substr(n + 1, m - n);

            textIndexFile += name;
            textIndexFile += "\n";
        }

        string indexPath = INDEX_FILE_PATH;
        int ind = open(indexPath.c_str(), O_APPEND | O_RDWR);
        if (ind < 0)
        {
            cout << " Error in opening the output file.\n";
            return;
        }

        lseek(ind, 0, SEEK_END);
        write(ind, textIndexFile.c_str(), textIndexFile.size());
    }
    else
    {
        for (int i = 1; i < commands.size(); i++)
        {
            fileNames.push_back(commands[i]);
        }
        vector<string> fileShas;
        string textIndexFile = "";
        for (int i = 0; i < fileNames.size(); i++)
        {
            string sha;
            if (isDirectory(fileNames[i]))
            {
                sha = calculateFolderSHA1(fileNames[i]);
                textIndexFile += "040000 tree ";
            }
            else
            {
                sha = calculateFileSHA1(fileNames[i]);
                textIndexFile += "100644 blob ";
            }

            fileShas.push_back(sha);

            textIndexFile += sha;
            textIndexFile += " ";
            textIndexFile += fileNames[i];
            textIndexFile += "\n";
        }

        string indexPath = INDEX_FILE_PATH;
        int ind = open(indexPath.c_str(), O_APPEND | O_RDWR);
        if (ind < 0)
        {
            cout << " Error in opening the output file.\n";
            return;
        }

        lseek(ind, 0, SEEK_END);
        write(ind, textIndexFile.c_str(), textIndexFile.size());
    }
}