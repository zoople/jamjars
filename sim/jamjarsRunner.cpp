#include <time.h>   
#include <math.h>
#include <stdio.h>   

#define NUMROWS 5
#define NUMCOLS 5
#define NUMSYMBOLS 3

class Rng {
	unsigned long long initialSeed;
	unsigned long long nextSeed;

	public:
		void setSeed(int seed) {initialSeed = seed;}
		unsigned long long generateNext() { nextSeed = ( nextSeed * 654188429 ) % 899809343;}
		unsigned long long getNext() { return nextSeed;}
		int getNext(int mod) { return nextSeed % mod;}

} ;


int printBoard(int (*gameBoard)[NUMCOLS])
{
	printf("----------------------------\n");

	int row,col =0;

	for (row = 0; row<NUMROWS; row++)
	{
		for (col = 0; col<NUMCOLS; col++)
		{
			printf("%i ", gameBoard[row][col]);

		}

		printf("\n");
	}


	printf("----------------------------\n");
	
}

int checkGame(int (*gameBoard)[NUMCOLS])
{
	
}

int createGame(int (*gameBoard)[NUMCOLS], int seed)
{
	int row,col =0;
	unsigned long long last = seed;

	Rng tileRng;
	tileRng.setSeed(seed);

	for (row = 0; row<NUMROWS; row++)
	{
		for (col = 0; col<NUMCOLS; col++)
		{
			tileRng.generateNext();

			gameBoard[row][col] = tileRng.getNext(NUMSYMBOLS);

		}
	}

}

int main()
{
	int gameBoard[NUMROWS][NUMCOLS] = {0};

	createGame(gameBoard, 1);
	printBoard(gameBoard);



}