#include "LevelInfo.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{	
	if (argc == 1) {
		cout << "leveltester: Received no arguments." << endl;
	}
	bool success = true;
	for (int i = 1; i < argc; i++) {
		LevelInfo lvl;
		if (lvl.loadFromFile(argv[i])) {
			success = false;
		}
	}
	cout << endl;
	cout << "leveltester done." << endl;
	if (success) {
		return 0;
	} else {
		return 1;
	}	
}
