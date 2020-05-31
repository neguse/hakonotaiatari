// pakcer.cc
//

#include <Windows.h>
#include <cdbpp.h>
#include <fstream>
#include <stdio.h>

#include <sys/types.h>
#include <dirent.h>

static const char* PACK_DIR = "./data/";
static const char* PACK_FILE = "data.dat";

using namespace std;

int main(int argc, char* argv[])
{
	std::ofstream os(PACK_FILE, std::ios::binary);
	if (os.fail()) {
		printf("fail to open pack_file\n");
		return -1;
	}
	cdbpp::builder b(os);

	DIR* pDir = opendir(PACK_DIR);
	if (NULL == pDir) {
		printf("fail to open pack_dir\n");
		return -2;
	}
	dirent* dir = NULL;

	while ((dir = readdir(pDir)) != NULL ){
		const char* file_name = dir->d_name;

		if (strcmp(file_name, ".") == 0) continue;
		if (strcmp(file_name, "..") == 0) continue;

		char file_path[1024];
		sprintf(file_path, "%s/%s", PACK_DIR, file_name);

		int length;
		char * buffer;

		ifstream is;
		is.open (file_path, ios::binary );

		// get length of file:
		is.seekg (0, ios::end);
		length = is.tellg();
		is.seekg (0, ios::beg);

		// allocate memory:
		buffer = new char [length];

		// read data as a block:
		is.read (buffer,length);
		is.close();

		b.put(file_name, strlen(file_name), buffer, length);
		delete[] buffer;
	}


	return 0;
}

