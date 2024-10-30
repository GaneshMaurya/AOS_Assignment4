#include <bits/stdc++.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
using namespace std;

#include "header.h"

void checkoutFiles(string path)
{
    int tempFd = open(path.c_str(), O_RDONLY);
    if (tempFd < 0)
    {
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;
    string fileContent = "";

    while ((bytesRead = read(tempFd, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytesRead] = '\0';
        fileContent += string(buffer);
    }
    close(tempFd);

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
        // int ffd = createFolder(folderPath);
        // if (ffd < 0 && errno != EEXIST)
        // {
        //     continue;
        // }

        string fileName = sha.substr(2);
        string binFilePath = folderPath + "/" + fileName;

        string decompPath = SRC_FOLDER + "/" + filePath;
        int fd = createFile(decompPath);
        fd = open(decompPath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd < 0)
        {
            continue;
        }

        // cout << decompPath << " <- " << binFilePath << "\n";

        decompressToFile(binFilePath.c_str(), fd);
        close(fd);
    }
}

void handleCheckout(vector<string> commands)
{
    string commitSha = commands[1];
    string folderName = commitSha.substr(0, 2);
    string folderPath = OBJECTS_DIR + folderName;

    string fileName = commitSha.substr(2, 38);
    string binFilePath = folderPath + "/" + fileName;

    // Check if file is already present
    DIR *directory = opendir(folderPath.c_str());
    if (directory == NULL)
    {
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
    // closedir(directory);

    if (!fileExists)
    {
        cout << "Hash does not exist.\n";
        return;
    }

    // Decompress and store commit metadata in another temp file
    int srcDir = createFolder(SRC_FOLDER);
    string tempFile = SRC_FOLDER + "/temp.txt";
    int nfd = createFile(tempFile);
    if (nfd < 0)
    {
        cout << "Error in creating the temp file.\n";
        return;
    }
    string commitFilePath = binFilePath;
    decompressToFile(commitFilePath.c_str(), nfd);
    close(nfd);

    // Decompress files present in temp.txt
    checkoutFiles(tempFile);

    // After decompressing files delete the temp file
    if (unlink(tempFile.c_str()) != 0)
    {
        cout << "Error in deleting the temp.txt file.\n";
        return;
    }

    // Update the master file
    int mfd = open(MASTER_FILE.c_str(), O_RDWR);
    if (mfd < 0)
    {
        cout << "Error in opening the file.\n";
        return;
    }
    lseek(mfd, 0, SEEK_SET);
    ftruncate(mfd, 0);
    write(mfd, commitSha.c_str(), commitSha.size());
    close(mfd);
}