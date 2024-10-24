make:
	g++ -o mygit main.cpp init.cpp hashObject.cpp catFile.cpp writeTree.cpp utils.cpp -lssl -lcrypto -lz
	clear