void initializeGit(vector<string> commands);

void handleHash(vector<string> commands);

void handleCat(vector<string> commands);

void handleWriteTree(vector<string> commands);

void handleLsTree(vector<string> commands);

void handleAdd(vector<string> commands);

string calculateFileSHA1(const string &filePath);

string calculateFolderSHA1(const string &folderPath);

int createFolder(string path);

int createFile(string path);

void compress(const char *input, const char *output, string metadata);

void decompressPrint(const char *file);

int decompressSize(const char *file);

string decompressType(const char *file);

void decompressPrintNames(const char *file);

bool isDirectory(string path);

char *getCurrDir();