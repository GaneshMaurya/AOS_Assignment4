#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
using namespace std;

#include "header.h"

const string INDEX_FILE_PATH = ".mygit/index.txt";
const string OBJECTS_DIR = ".mygit/objects/";

void handleLsTree(vector<string> commands)
{

    if (commands.size() == 2)
    {
        string treeSha = commands[1];
        string folderName = treeSha.substr(0, 2);
        string fileName = treeSha.substr(2, 38);
        string filePath = OBJECTS_DIR + folderName + "/" + fileName;
        decompressPrint(filePath.c_str());
    }
    else
    {
        string flag = commands[1];
        string treeSha = commands[2];
        string folderName = treeSha.substr(0, 2);
        string fileName = treeSha.substr(2, 38);
        string filePath = OBJECTS_DIR + folderName + "/" + fileName;

        if (flag != "--name-only")
        {
            cout << "Please enter correct flag.\n";
            return;
        }
        else
        {
            decompressPrintNames(filePath.c_str());
        }
    }
}