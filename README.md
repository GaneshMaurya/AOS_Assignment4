## Assignment4
### G V Ganesh Maurya - 2024201030

## 1. Init
- Creates a .mygit folder
- The .mygit directory has folders namely logs, objects, refs and HEAD, index files.
- Objects will store the compressed object details
- Refs will store details related to current branch's sha
- Logs will store the commit history

```./mygit init```

## 2. Hash-Object
- If ```-w``` is not used then we just print the sha of that particular file.
- If ```-w``` flag is used in the command then it creates a ```blob``` object and stores it in the Objects folder.

```./mygit cat-file <flag> <file_sha>```

## 3. Write Tree
- Writes the current directory structure to a tree object, which represents the hierarchy
of files and directories.
- It also recursively traverses through the folders and add stores it.
- Creates a ```tree``` object.

```./mygit write-tree```

## 4. Ls Tree
- Reads the content of ```tree``` object and prints in the STD_OUT.
- If the flag ```--name-only``` is used then we have to print only the names which are present in the ```tree``` object.

```./mygit ls-tree [--name-only] <tree_sha>```

## 5. Add
- Adds the to the staging area, objects metadata into index file and creates ```blobs``` in the objects folder.

```./mygit add .```
```./mygit add <file-1> <file-2>```

## 6. Commit
- It creates a commit object in the objects folder, representing the staged changes.
- Updates the master file in refs/heads folder with the current commit sha.
- Updates the HEAD file in logs folder with commit details.
- This file keeps track of the commit history.

```./mygit commit -m "Commit message"```
```./mygit commit```

## 7. Log
- Displays the commit log. 
- It reads the HEAD file from logs folder and prints it's content.

```./mygit log```

## 8. Checkout
- A new folder called ```src``` is created in the current working directory.
- It restores the state to the previous commit which is mentioned in the arguments(commit_sha).

```./mygit checkout <commit_sha>```

### Note:
- ZLib algorithm is used for compression and decompression.