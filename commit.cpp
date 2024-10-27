#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <chrono>
using namespace std;

#include "header.h"

void createObjects(string path)
{
    int indexFd = open(path.c_str(), O_RDONLY);
    if (indexFd < 0)
    {
        // cout << "Error opening index file: " << path << "\n";
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    string fileContent = "";

    while ((bytesRead = read(indexFd, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytesRead] = '\0';
        fileContent += string(buffer);
    }
    close(indexFd);

    if (fileContent == "")
    {
        return;
    }

    stringstream ss(fileContent);
    string line;
    while (getline(ss, line))
    {
        istringstream lineStream(line);
        string permissions, type, sha, filePath;

        lineStream >> permissions >> type >> sha;
        getline(lineStream, filePath);
        if (filePath.empty())
        {
            cout << "Error: Invalid line format in index file: " << line << "\n";
            continue;
        }

        filePath.erase(0, filePath.find_first_not_of(" "));

        if (type != "blob")
        {
            continue;
        }

        string folderName = sha.substr(0, 2);
        string folderPath = OBJECTS_DIR + folderName;
        int ffd = createFolder(folderPath);
        if (ffd < 0 && errno != EEXIST)
        {
            continue;
        }

        string fileName = sha.substr(2);
        string binFilePath = folderPath + "/" + fileName;

        struct stat buffer;
        if (stat(binFilePath.c_str(), &buffer) == 0)
        {
            continue;
        }

        int fd = createFile(binFilePath);
        if (fd < 0)
        {
            // cout << "Error creating object file: " << binFilePath << "\n";
            continue;
        }

        int inputFd = open(filePath.c_str(), O_RDONLY);
        if (inputFd < 0)
        {
            cout << "Error opening source file: " << filePath << "\n";
            close(fd);
            continue;
        }

        off_t fileSize = lseek(inputFd, 0, SEEK_END);
        lseek(inputFd, 0, SEEK_SET);

        string metadata = "blob " + to_string(fileSize) + "\0";

        const char *inputFile = filePath.c_str();
        const char *outputFile = binFilePath.c_str();
        compress(inputFile, outputFile, metadata);

        close(inputFd);
        close(fd);
    }
}

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

    int size = lseek(ifd, 0, SEEK_END);
    lseek(ifd, 0, SEEK_SET);

    if (size == 0)
    {
        cout << "Nothing to commit.\n";
        return;
    }
    else
    {
        createObjects(INDEX_FILE_PATH);
    }

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