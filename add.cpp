#include <bits/stdc++.h>
using namespace std;

#include "header.h"

void handleAdd(vector<string> commands)
{
    if (commands[1] == ".")
    {
    }
    else
    {
        vector<string> fileNames;
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
                textIndexFile += "040000 tree";
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

        cout << textIndexFile << "\n";
    }
}