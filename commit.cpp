#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <chrono>
using namespace std;

#include "header.h"

void handleCommit(vector<string> commands)
{
    string commitMessage;

    if (commands.size() == 1)
    {
        // Default message
        commitMessage = "Commit";
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
            commitMessage = commands[2];
        }
    }

    int ifd = open(INDEX_FILE_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    if (ifd < 0)
    {
        cout << "Error in opening the index file.\n";
        close(ifd);
        return;
    }

    // createTreeObjects(INDEX_FILE_PATH);

    string commitSha = calculateFileSHA1(INDEX_FILE_PATH);
    string folderName = commitSha.substr(0, 2);
    string folderPath = OBJECTS_DIR + folderName;
    int ffd = createFolder(folderPath);
    if (ffd < 0 && errno != EEXIST)
    {
        cout << "Error in creating the folder.\n";
        return;
    }

    string fileName = commitSha.substr(2, 38);
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
            cout << commitSha << "\n";
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
    close(ffd);
    close(fd);

    cout << commitSha << "\n";

    // Store commit details in refs
    int mfd = open(MASTER_FILE.c_str(), O_RDWR);
    if (mfd < 0)
    {
        cout << "Error in opening the file.\n";
        return;
    }

    string parentCommitSha = "0000000000000000000000000000000000000000";
    char parent[BUFFER_SIZE];
    int bytesRead = read(mfd, parent, sizeof(parent) - 1);
    if (bytesRead > 0)
    {
        parent[bytesRead] = '\0';
        parentCommitSha.assign(parent, bytesRead);
    }
    lseek(mfd, 0, SEEK_SET);
    ftruncate(mfd, 0);
    write(mfd, commitSha.c_str(), commitSha.size());

    // Store commit details in log files
    int hfd = open(LOG_HEAD.c_str(), O_APPEND | O_RDWR);
    if (hfd < 0)
    {
        cout << "Error in opening the file.\n";
        return;
    }

    string headData = parentCommitSha;
    headData += " ";
    headData += commitSha;
    headData += " Ganesh Maurya <gvganesh.maurya@students.iiit.ac.in> ";

    auto now = chrono::system_clock::now();
    time_t timestamp = chrono::system_clock::to_time_t(now);
    headData += to_string(timestamp);

    headData += " +0530	commit: ";
    headData += commitMessage;
    headData += "\n";
    write(hfd, headData.c_str(), headData.size());

    ftruncate(ifd, 0);
    close(mfd);
    close(ifd);
}