#include <bits/stdc++.h>
using namespace std;

#include "header.h"

int main(int argc, char *argv[])
{
    vector<string> commands;
    for (int i = 1; i < argc; i++)
    {
        commands.push_back(argv[i]);
    }

    if (commands[0] == "init")
    {
        initializeGit(commands);
    }
    else if (commands[0] == "hash-object")
    {
        handleHash(commands);
    }
    else if (commands[0] == "cat-file")
    {
        handleCat(commands);
    }
    else if (commands[0] == "write-tree")
    {
        handleWriteTree(commands);
    }
    else if (commands[0] == "ls-tree")
    {
        handleLsTree(commands);
    }
    else if (commands[0] == "add")
    {
        handleAdd(commands);
    }
    else if (commands[0] == "commit")
    {
        handleCommit(commands);
    }
    else if (commands[0] == "log")
    {
    }
    else if (commands[0] == "checkout")
    {
    }
    else
    {
        cout << "Command does not exist\n";
    }

    return 0;
}