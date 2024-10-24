#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

#include "header.h"

void initializeGit(vector<string> commands)
{
    string path = ".mygit";
    int gitDir = createFolder(path);

    // Create index file to store details about files that are added and to be committed
    string indexPath = path + "/index.txt";
    int indexFile = createFile(indexPath);
    if (indexFile < 0)
    {
        cout << "Error in creating the index file.\n";
        close(indexFile);
        return;
    }

    // Objects folder (To store the blob, tree, commit objects)
    string objectsFolder = path + "/objects";
    int objectsDir = createFolder(objectsFolder);

    // HEAD File
    string headPath = path + "/HEAD.txt";
    int headFile = createFile(headPath);
    if (headFile < 0)
    {
        cout << "Error in creating the HEAD file.\n";
        close(headFile);
        return;
    }

    // Refs folder (To store the commit history and other related data)
    string refsFolder = path + "/refs";
    int refsDir = createFolder(refsFolder);

    string headsFolder = refsFolder + "/heads";
    int headsDir = createFolder(headsFolder);

    string commitFolder = headsFolder + "/master";
    int commitDir = createFolder(commitFolder);

    // Initialize HEAD file
    string buffer = "ref: refs/heads/master";
    int valWrite = write(headFile, buffer.c_str(), buffer.size());
    if (valWrite < 0)
    {
        cout << "Error in writing content onto the file.\n";
        return;
    }

    close(indexFile);
    close(headFile);

    cout << "A " << path << " directory is created with the required structure for the repository.\n";
}