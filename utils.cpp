#include <bits/stdc++.h>
#include <openssl/sha.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zlib.h>
#include <dirent.h>
using namespace std;

const int BUFFER_SIZE = 8 * 1024;

string calculateFileSHA1(const string &filePath)
{
    unsigned char buffer[BUFFER_SIZE];
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA_CTX shaContext;

    SHA1_Init(&shaContext);

    int fileDescriptor = open(filePath.c_str(), O_RDONLY);
    if (fileDescriptor < 0)
    {
        cout << "Error opening file: " << filePath << "\n";
        return "";
    }

    ssize_t bytesRead;
    while ((bytesRead = read(fileDescriptor, buffer, BUFFER_SIZE)) > 0)
    {
        SHA1_Update(&shaContext, buffer, bytesRead);
    }

    SHA1_Final(hash, &shaContext);
    close(fileDescriptor);

    stringstream ss;
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}

bool isDirectory(string path)
{
    struct stat dirStat;
    if (stat(path.c_str(), &dirStat) == -1)
    {
        cout << "Error in getting the stat of file/folder.\n";
        return false;
    }

    if (S_ISDIR(dirStat.st_mode))
    {
        return true;
    }

    return false;
}

void dfs(string folderName, vector<string> &hashes)
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
                closedir(directory);
            }
            else
            {
                string fileSha = calculateFileSHA1(path);
                hashes.push_back(fileSha);
            }
        }

        dirInfo = readdir(directory);
    }
    closedir(directory);
    return;
}

string calculateFolderSHA1(const string &folderPath)
{
    vector<string> hashes;
    dfs(folderPath, hashes);
    sort(hashes.begin(), hashes.end());

    SHA_CTX sha1;
    SHA1_Init(&sha1);
    for (const auto &hash : hashes)
    {
        SHA1_Update(&sha1, hash.c_str(), hash.size());
    }

    unsigned char final_hash[SHA_DIGEST_LENGTH];
    SHA1_Final(final_hash, &sha1);

    ostringstream fss;
    for (unsigned char c : final_hash)
    {
        fss << hex << setw(2) << setfill('0') << (int)c;
    }
    return fss.str();
}

int createFolder(string path)
{
    int dir = mkdir(path.c_str(), 0700);
    chmod(path.c_str(), 0700);
    if (dir < 0 && errno == EEXIST)
    {
        rmdir(path.c_str());
        mkdir(path.c_str(), 0700);
        chmod(path.c_str(), 0700);
    }

    return dir;
}

int createFile(string path)
{
    int file = open(path.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (file < 0)
    {
        cout << "Error in creating the index file.\n";
        close(file);
        return -1;
    }

    return file;
}

void compress(const char *input, const char *output)
{
    int ipFile = open(input, O_RDWR, S_IRUSR | S_IWUSR);
    if (ipFile < 0)
    {
        cout << "Error in creating the index file.\n";
        close(ipFile);
        return;
    }

    int opFile = open(output, O_RDWR, S_IRUSR | S_IWUSR);
    if (opFile < 0)
    {
        cout << "Error in creating the index file.\n";
        close(opFile);
        return;
    }

    z_stream zs;
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;

    if (deflateInit(&zs, Z_DEFAULT_COMPRESSION) != Z_OK)
    {
        cout << "Error in initialization of compression\n";
        return;
    }

    char inputBuffer[BUFFER_SIZE];
    char outputBuffer[BUFFER_SIZE];

    int bytesRead;
    while ((bytesRead = read(ipFile, inputBuffer, sizeof(inputBuffer))) > 0)
    {
        zs.avail_in = static_cast<uInt>(bytesRead);
        zs.next_in = reinterpret_cast<Bytef *>(inputBuffer);

        do
        {
            zs.avail_out = sizeof(outputBuffer);
            zs.next_out = reinterpret_cast<Bytef *>(outputBuffer);

            deflate(&zs, Z_FINISH);

            int bytesToWrite = sizeof(outputBuffer) - zs.avail_out;

            if (write(opFile, outputBuffer, bytesToWrite) != bytesToWrite)
            {
                cout << "Error in writing to the output file\n";
                deflateEnd(&zs);
                close(ipFile);
                close(opFile);
                return;
            }
        } while (zs.avail_out == 0);
    }

    deflateEnd(&zs);
    close(ipFile);
    close(opFile);

    return;
}

void decompressPrint(const char *file)
{
    int fd = open(file, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        cout << "Error in creating the input file.\n";
        close(fd);
        return;
    }

    z_stream zs;
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;

    if (inflateInit(&zs) != Z_OK)
    {
        cout << "Error in initialization of decompression\n";
        return;
    }

    char inputBuffer[BUFFER_SIZE];
    char outputBuffer[BUFFER_SIZE];

    bool flag = false;
    string metadata;
    int bytesRead;
    while ((bytesRead = read(fd, inputBuffer, sizeof(inputBuffer))) > 0)
    {
        zs.avail_in = static_cast<uInt>(bytesRead);
        zs.next_in = reinterpret_cast<Bytef *>(inputBuffer);

        do
        {
            zs.avail_out = sizeof(outputBuffer);
            zs.next_out = reinterpret_cast<Bytef *>(outputBuffer);

            int status = inflate(&zs, Z_NO_FLUSH);
            if (status == Z_STREAM_END || status == Z_OK)
            {
                int bytesToWrite = sizeof(outputBuffer) - zs.avail_out;

                if (flag == false)
                {
                    for (int i = 0; i < bytesToWrite; ++i)
                    {
                        if (outputBuffer[i] == '$')
                        {
                            flag = true;
                            metadata.append(outputBuffer, i + 1);

                            if (i + 1 < bytesToWrite)
                            {
                                write(STDOUT_FILENO, &outputBuffer[i + 1], bytesToWrite - (i + 1));
                            }
                            break;
                        }
                    }

                    if (flag == false)
                    {
                        metadata.append(outputBuffer, bytesToWrite);
                    }
                }
                else
                {
                    if (write(STDOUT_FILENO, outputBuffer, bytesToWrite) != bytesToWrite)
                    {
                        cout << "Failed to write to standard output\n";
                        inflateEnd(&zs);
                        close(fd);
                        return;
                    }
                }

                if (status == Z_STREAM_END)
                {
                    break;
                }
            }
            else
            {
                cout << "Decompression error: " << status << "\n";
                inflateEnd(&zs);
                close(fd);
                return;
            }

        } while (zs.avail_out == 0);
    }

    inflateEnd(&zs);
    close(fd);

    return;
}

int decompressSize(const char *file)
{
    int fd = open(file, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        cout << "Error in creating the input file.\n";
        close(fd);
        return -1;
    }

    z_stream zs;
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;

    if (inflateInit(&zs) != Z_OK)
    {
        cout << "Error in initialization of decompression\n";
        return -1;
    }

    char inputBuffer[BUFFER_SIZE];
    char outputBuffer[BUFFER_SIZE];

    bool flag = false;
    string metadata;
    int bytesRead;
    while ((bytesRead = read(fd, inputBuffer, sizeof(inputBuffer))) > 0)
    {
        zs.avail_in = static_cast<uInt>(bytesRead);
        zs.next_in = reinterpret_cast<Bytef *>(inputBuffer);

        do
        {
            zs.avail_out = sizeof(outputBuffer);
            zs.next_out = reinterpret_cast<Bytef *>(outputBuffer);

            int status = inflate(&zs, Z_NO_FLUSH);
            if (status == Z_STREAM_END || status == Z_OK)
            {
                int bytesToWrite = sizeof(outputBuffer) - zs.avail_out;

                if (flag == false)
                {
                    for (int i = 0; i < bytesToWrite; ++i)
                    {
                        if (outputBuffer[i] == '$')
                        {
                            flag = true;
                            metadata.append(outputBuffer, i + 1);
                            break;
                        }
                    }

                    if (flag == false)
                    {
                        metadata.append(outputBuffer, bytesToWrite);
                    }
                }

                if (status == Z_STREAM_END)
                {
                    break;
                }
            }
            else
            {
                cout << "Decompression error: " << status << "\n";
                inflateEnd(&zs);
                close(fd);
                return -1;
            }

            if (flag == true)
            {
                break;
            }

        } while (zs.avail_out == 0);
    }

    inflateEnd(&zs);
    close(fd);

    int n = metadata.size();
    int totalSize = stoi(metadata.substr(5, n - 2));

    return totalSize;
}

string decompressType(const char *file)
{
    int fd = open(file, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        cout << "Error in creating the input file.\n";
        close(fd);
        return "";
    }

    z_stream zs;
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;

    if (inflateInit(&zs) != Z_OK)
    {
        cout << "Error in initialization of decompression\n";
        return "";
    }

    char inputBuffer[BUFFER_SIZE];
    char outputBuffer[BUFFER_SIZE];

    bool flag = false;
    string metadata;
    int bytesRead;
    while ((bytesRead = read(fd, inputBuffer, sizeof(inputBuffer))) > 0)
    {
        zs.avail_in = static_cast<uInt>(bytesRead);
        zs.next_in = reinterpret_cast<Bytef *>(inputBuffer);

        do
        {
            zs.avail_out = sizeof(outputBuffer);
            zs.next_out = reinterpret_cast<Bytef *>(outputBuffer);

            int status = inflate(&zs, Z_NO_FLUSH);
            if (status == Z_STREAM_END || status == Z_OK)
            {
                int bytesToWrite = sizeof(outputBuffer) - zs.avail_out;

                if (flag == false)
                {
                    for (int i = 0; i < bytesToWrite; ++i)
                    {
                        if (outputBuffer[i] == '$')
                        {
                            flag = true;
                            metadata.append(outputBuffer, i + 1);
                            break;
                        }
                    }

                    if (flag == false)
                    {
                        metadata.append(outputBuffer, bytesToWrite);
                    }
                }

                if (status == Z_STREAM_END)
                {
                    break;
                }
            }
            else
            {
                cout << "Decompression error: " << status << "\n";
                inflateEnd(&zs);
                close(fd);
                return "";
            }

            if (flag == true)
            {
                break;
            }

        } while (zs.avail_out == 0);
    }

    inflateEnd(&zs);
    close(fd);

    string type = metadata.substr(0, 4);
    return type;
}