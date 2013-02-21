#include "cavegen.h"

using namespace std;

char grid[W][H];

void doExampleRandWalk(float fill, int turnRes) {
	cout << "RandWalk\n\n";
	cout << "turn resistance: " << turnRes << endl << endl;
	cout << "fill rate: " << fill << endl << endl;
	fillGrid(grid);
	randWalk(grid, fill, turnRes);
	removeLonelyWalls(grid);
	cout << "After removing lonely walls, no smoothing: \n\n";
	printGrid(grid);
}

void doExampleAutomaton(int smoothCycles) {
	cout << "Automaton\n\n";
	fillGridWithNoise(grid);
	CaveAutomaton(grid);
	SmoothingAutomaton(grid);
	removeLonelyWalls(grid);
	printGrid(grid);
	cout << endl;
	while(smoothCycles > 0) {
		cout << "After additional smoothing:\n\n";
		SmoothingAutomaton(grid);
		printGrid(grid);
		cout << endl;
		smoothCycles--;
	}
}

int main(int argc, char *argv[]) {
	unsigned int seed = static_cast<unsigned int>(time(NULL));
	//seed = ;
	srand(seed);
	
	for(int i = 0; i < 20; i++) {
		cout << "SEED: " << seed << endl << endl;
		doExampleRandWalk(0.45, 4);
		//doExampleAutomaton(1);
		seed = xorRand();
		srand(seed);
	}
	
	return 0;
}