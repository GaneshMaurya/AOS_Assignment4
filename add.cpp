#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
using namespace std;

#include "header.h"

void dfs(string folderName, vector<string> &files)
{
    DIR *directory = opendir(folderName.c_str());
    if (directory == NULL)
    {
        return;
    }

    struct dirent *dirInfo;
    while ((dirInfo = readdir(directory)) != NULL)
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
    }
    closedir(directory);
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

    if (fileContent.empty())
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
            // createTree(filePath);
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

        int iFd = open(filePath.c_str(), O_RDONLY);
        if (iFd < 0)
        {
            cout << "Error opening source file: " << filePath << "\n";
            close(fd);
            continue;
        }

        off_t fileSize = lseek(iFd, 0, SEEK_END);
        lseek(iFd, 0, SEEK_SET);

        string metadata = "blob " + to_string(fileSize) + "$";

        const char *inputFile = filePath.c_str();
        const char *outputFile = binFilePath.c_str();
        compress(inputFile, outputFile, metadata);

        close(iFd);
        close(fd);
    }
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

    int ifd = open(INDEX_FILE_PATH.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
    if (ifd < 0)
    {
        cout << "Error in opening the index file.\n";
        close(ifd);
        return;
    }

    int size = lseek(ifd, 0, SEEK_END);
    lseek(ifd, 0, SEEK_SET);

    if (size != 0)
    {
        createObjects(INDEX_FILE_PATH);
    }

    close(ifd);
}