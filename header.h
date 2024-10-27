const int BUFFER_SIZE = 8 * 1024;
const string INDEX_FILE_PATH = ".mygit/index";
const string OBJECTS_DIR = ".mygit/objects/";
const string MASTER_FILE = ".mygit/refs/heads/master";
const string LOG_HEAD = ".mygit/logs/HEAD";

void initializeGit(vector<string> commands);

void handleHash(vector<string> commands);

void handleCat(vector<string> commands);

void handleWriteTree(vector<string> commands);

void handleLsTree(vector<string> commands);

void handleAdd(vector<string> commands);

void handleCommit(vector<string> commands);

void handleLog(vector<string> commands);

string calculateFileSHA1(const string &filePath);

string calculateFolderSHA1(const string &folderPath);

string calculateStringSHA1(const string &input);

int createFolder(string path);

int createFile(string path);

void compress(const char *input, const char *output, string metadata);

void decompressPrint(const char *file);

int decompressSize(const char *file);

string decompressType(const char *file);

void decompressPrintNames(const char *file);

bool isDirectory(string path);

void dfs(string folderName, vector<string> &files);

char *getCurrDir();