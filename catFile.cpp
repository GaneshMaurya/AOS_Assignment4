#include <bits/stdc++.h>
using namespace std;

#include "header.h"

void handleCat(vector<string> commands)
{
    // Check if the flag is present or not
    if (commands.size() == 3)
    {
        string flag = commands[1];
        string sha = commands[2];

        string folderName = sha.substr(0, 2);
        string fileName = sha.substr(2, 38);
        string filePath = OBJECTS_DIR + folderName + "/" + fileName;

        if (flag == "-p")
        {
            decompressPrint(filePath.c_str());
            cout << "\n";
        }
        else if (flag == "-s")
        {
            int size = decompressSize(filePath.c_str());
            if (size <= -1)
            {
                cout << "Error in printing the size\n";
                return;
            }
            cout << size << "\n";
        }
        else if (flag == "-t")
        {
            string type = decompressType(filePath.c_str());
            if (type == "")
            {
                cout << "Error in printing the type\n";
                return;
            }
            cout << type << "\n";
        }
        else
        {
            cout << "Entered flag value is wrong";
            return;
        }
    }
    else
    {
        cout << "Wrong commands entered. Please retry.\n";
    }
}