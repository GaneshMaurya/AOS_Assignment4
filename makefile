make:
	g++ -o mygit main.cpp init.cpp hashObject.cpp catFile.cpp writeTree.cpp add.cpp utils.cpp -lssl -lcrypto -lz
	cd testingFolder
	clear