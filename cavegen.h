#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>

#ifndef CAVEGEN_INCLUDED
#define CAVEGEN_INCLUDED

using namespace std;
#define H 64
#define W 64

struct coord {
	int x;
	int y;
};

//very fast XOR Shift random number generator.
//The first time it is called, x is seeded by rand().
//Based on code from wikipedia: <http://en.wikipedia.org/wiki/Xorshift>
uint32_t xorRand() {
	static uint32_t x = (uint32_t)rand(); 
	static uint32_t y = 362436069;
	static uint32_t z = 521288629;
	static uint32_t w = 88675123;
	uint32_t t;
	 
	t = x ^ (x << 11);
	x = y; y = z; z = w; 
	return w = w ^ (w >> 19) ^ (t ^ (t >> 8));
}

// Generate random int from lo to hi, inclusive 
int randInt(int lo, int hi) {
	return lo + xorRand() % (hi - lo + 1); 
}

//return true with a given probability 
//slow because it uses rand() and not xorRand(). 
//TODO: adapt it to use xorRand()
bool trueWithProbability(double p) {
    return rand() < p * RAND_MAX + p;
}

//prints the grid to the console
void printGrid(char grid[W][H]){
	for(int k = 0; k < H; k++) {
		for(int j = 0; j < W; j++) {
			cout << grid[j][k];
		}
		cout << endl;
	}
	
}

//fills the interior of the grid with '%', and the edges with '#'
//used for initialization for the random walk algorithm.
void fillGrid(char grid[W][H]){
	for(int k = 0; k < H; k++) {
		for(int j = 0; j < W; j++) { 
			//outer wall boundaries
			if(j == 0 || j == W-1 || k == 0 || k == H-1) {
				grid[j][k] = '#';
			}
			else { //interior
				grid[j][k] = '%';
			}
		}
	}
}

//Instead of filling every interior space with '%', it chooses
//between ' ' and '%' with a goal of 45% ' ' and 55% '%'.
//Used for initialization for the cellular automaton algorithm.
void fillGridWithNoise(char grid[W][H]){
	for(int k = 0; k < H; k++) {
		for(int j = 0; j < W; j++) { 
			//outer wall boundaries
			if(j == 0 || j == W-1 || k == 0 || k == H-1) {
				grid[j][k] = '#';
			}
			else { //interior
				if(trueWithProbability(0.45)) {
					grid[j][k] = ' ';
				}
				else {
					grid[j][k] = '%';
				}
			}
		}
	}
}

//Places a random point on the grid, then repeatedly 'takes a step' from that point,
//making a floor tile wherever it visits. 
//End condition: not sure yet. For now, keeps track of the number of floor tiles
//and stops when that number approaches a certain percentage of the total grid.
//A good value for the fill percentage seems to be 0.45
void randWalk(char grid[W][H], double fill, int turnResistance){
	coord pos; //position
	pos.x = randInt(1, W-2);
	pos.y = randInt(1, H-2);
	grid[pos.x][pos.y] = ' ';
	vector<coord> floorTiles;
	int dir = 0;

	while(floorTiles.size() < (unsigned int)(fill*(H-1)*(W-1))) {
		//continue in the same direction with a chance that depends on turn resistance.
		//higher turn resistance means it is more likely to continue in a certain direction
		//rather than change directions often. A value of 4 tends to look pretty good.
		if(xorRand() % turnResistance == 0) { 
			//choose a new direction
			dir = xorRand() % 4;
		}
		
		switch(dir){
			case 0: //try to move up
				if(grid[pos.x][pos.y - 1] != '#') {
					pos.y -= 1;
					if(grid[pos.x][pos.y] != ' ') {
						grid[pos.x][pos.y] = ' ';
						floorTiles.push_back(pos);
					}
				} 
				break;
			case 1: //try to move down
				if(grid[pos.x][pos.y + 1] != '#') {
					pos.y += 1;
					if(grid[pos.x][pos.y] != ' ') {
						grid[pos.x][pos.y] = ' ';
						floorTiles.push_back(pos);
					}
				}
				break;
			case 2: //try to move left
				if(grid[pos.x - 1][pos.y] != '#') {
					pos.x -= 1;
					if(grid[pos.x][pos.y] != ' ') {
						grid[pos.x][pos.y] = ' ';
						floorTiles.push_back(pos);
					}
				}
				break;
			case 3: //try to move right
				if(grid[pos.x + 1][pos.y] != '#') {
					pos.x += 1;
					if(grid[pos.x][pos.y] != ' ') {
						grid[pos.x][pos.y] = ' ';
						floorTiles.push_back(pos);
					}
				}
				break;
			default: break;
		}
	}
}

//Takes a location on the grid and returns the number of 'live' neighbors it has
//in the adjacent 8 spaces. A 'live' neighbor is anything other than ' '.
//  . . .
//  . + .
//  . . .
int getNumNeighbors(char grid[W][H], coord pos) {
	int n = 0;
	if(grid[pos.x+1][pos.y] != ' ') {
		n++;
	}
	if(grid[pos.x+1][pos.y+1] != ' ') {
		n++;
	}
	if(grid[pos.x+1][pos.y-1] != ' ') {
		n++;
	}
	if(grid[pos.x-1][pos.y] != ' ') {
		n++;
	}
	if(grid[pos.x-1][pos.y+1] != ' ') {
		n++;
	}
	if(grid[pos.x-1][pos.y-1] != ' ') {
		n++;
	}
	if(grid[pos.x][pos.y+1] != ' ') {
		n++;
	}
	if(grid[pos.x][pos.y-1] != ' ') {
		n++;
	}
	return n;
}

//removes 'live' wall tiles if they have no neighbors. This is useful for 
//cleaning up after the random walk algorithm runs.
void removeLonelyWalls(char grid[W][H]) {
	coord pos;
	for(pos.y = 0; pos.y < H; pos.y++) {
		for(pos.x = 0; pos.x < W; pos.x++) { 
			//if not an outer wall and has no neighbors
			if(!(pos.x == 0 || pos.x == W-1 || pos.y == 0 || pos.y == H-1) 
			  && getNumNeighbors(grid, pos) == 0) {
				grid[pos.x][pos.y] = ' ';
			}
		}
	}
}

//checks if the cell at coordinate 'pos' in the grid should survive
//by comparing its number of neighbors to the survRate. if number of 
//neighbors is equal or greater than the survRate, then the cell lives
bool CellSurvived(char grid[W][H], coord pos, int survRate) {
	return (getNumNeighbors(grid, pos) >= survRate);
}

//Checks if a cell should be born at coordinate 'pos' by comparing 
//its number of neighbors to the given birthRate. if number of neighbors
//is equal or greater than the birthRate, a cell is 'born'
bool CellGaveBirth(char grid[W][H], coord pos, int birthRate) {
	return (getNumNeighbors(grid, pos) >= birthRate);

}

void removeDeadCells(char grid[W][H], vector<coord> dead) {
	coord pos;
	while(!dead.empty()) {
		pos = dead.back();
		grid[pos.x][pos.y] = ' ';
		dead.pop_back();
	}
}

void addBornCells(char grid[W][H], vector<coord> born) {
	coord pos;
	while(!born.empty()) {
		pos = born.back();
		grid[pos.x][pos.y] = '%';
		born.pop_back();
	}
}

//Apply the cellular automaton method: 
//Born if 6, 7, 8 neighbors
//Survive if 3, 4, 5, 6, 7, 8 neighbors
//repeat the automaton ~20 iterations so it will settle
void CaveAutomaton(char grid[W][H]) {
	coord pos;
	//keeps track of every coord that will have a 'birth'
	vector<coord> bornList;
	//keeps track of every coord that 'died' i.e. needs to be changed to ' '.
	vector<coord> diedList;

	for(int i = 0; i < 20; i++) {
		removeDeadCells(grid, diedList);
		addBornCells(grid, bornList);
		diedList.clear();
		bornList.clear();
		//for(pos.y = 0; pos.y < H; pos.y++) {
		do {
			for(pos.x = 0; pos.x < W; pos.x++) { 
				//if not an outer wall
				if(!(pos.x == 0 || pos.x == W-1 || pos.y == 0 || pos.y == H-1)) {
					if(!CellSurvived(grid, pos, 3) && grid[pos.x][pos.y] == '%') {
						diedList.push_back(pos);
					}
					if(CellGaveBirth(grid, pos, 6) && grid[pos.x][pos.y] == ' ') {
						bornList.push_back(pos);
					}
				}
			}
		} while (!bornList.empty() && !diedList.empty());
		//cout << "automaton cycle" << endl;
	} 
}

//Apply the smoothing automaton: B5678/S5678
void SmoothingAutomaton(char grid[W][H]) {
	coord pos;
	//keeps track of every coord that will have a 'birth'
	vector<coord> bornList;
	//keeps track of every coord that 'died' i.e. needs to be changed to ' '.
	vector<coord> diedList;
	
	for(pos.y = 0; pos.y < H; pos.y++) {
		for(pos.x = 0; pos.x < W; pos.x++) { 
			//if not an outer wall
			if(!(pos.x == 0 || pos.x == W-1 || pos.y == 0 || pos.y == H-1)) {
				if(!CellSurvived(grid, pos, 5)) {
					diedList.push_back(pos);
				}
				if(CellGaveBirth(grid, pos, 5)) {
					bornList.push_back(pos);
				}
			}
		}
	}
	removeDeadCells(grid, diedList);
	addBornCells(grid, bornList);
	diedList.clear();
	bornList.clear();
}

#endif //CAVEGEN_INCLUDED