make:
	g++ -o mygit main.cpp init.cpp hashObject.cpp catFile.cpp writeTree.cpp lsTree.cpp add.cpp commit.cpp log.cpp checkout.cpp utils.cpp -lssl -lcrypto -lz
	clear