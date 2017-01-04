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
		void setSeed(int seed) {initialSeed = seed; nextSeed = 1;}
	unsigned long long generateNext() { 
		int skip = -1;
		if (nextSeed == 1) skip = 1; else skip = 0;

		for (int times = 0; times<1+skip; times++) nextSeed = ( nextSeed * 654188429 ) % 899809343;}
	//	unsigned long long generateNext() { nextSeed = ( nextSeed * 5 ) % 11;}
		
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







int checkSquare(int (*gameBoard)[NUMCOLS], int (*markBoard)[NUMCOLS], int x, int y)
{
	//printf("SQUAURE %i, %i\n", x, y);
	markBoard[x][y] = gameBoard[x][y];

	int found = 1;
	int xdelta = 0;
	int ydelta = 0;

	int deltalist[4][2] = 
	{
		{-1, 0},
		{1, 0},
		{0, -1},
		{0, 1}
	};

	for (int delta = 0; delta<4; delta++)
	{	
		xdelta = deltalist[delta][0];
		ydelta = deltalist[delta][1];
		 

		// printf("checking %i, %i\n", x+xdelta, y+ydelta);

		 if (y+ydelta >= 0 && x+xdelta >= 0){ 
		 	if (markBoard[x+xdelta][y+ydelta] == 0) //check if you can go there
		 	{
		 		//printf("I found %i\n", gameBoard[x+xdelta][y+ydelta]);
		 		if (gameBoard[x+xdelta][y+ydelta] == gameBoard[x][y]) {
		 			//printf ("That's a match, so I'll see if there are any more\n");
		 			found += checkSquare(gameBoard, markBoard, x+xdelta, y+ydelta); 
		 		}
		 		

		 	} //else printf("..............already been there\n");
		 } //else printf("..............past edge\n");


	}

	return found;
}

int checkGame(int (*gameBoard)[NUMCOLS])
{
	int markBoard[NUMROWS][NUMCOLS] = {{0}};
	int clearBoard[NUMROWS][NUMCOLS] = {{0}};

	int clusterLength = 1;

	int winStarts[NUMCOLS*NUMROWS][2] = {{0}};
	int numWins = 0;


	for (int x=0; x<NUMCOLS; x++)
	for (int y=0; y<NUMCOLS; y++)
	{
		if (markBoard[x][y] == 0)
		{
			clusterLength = checkSquare(gameBoard, markBoard, x, y);
			printf("(%i, %i), Symbol = %i, Number =  %i\n", x, y, gameBoard[x][y],clusterLength);
			if (clusterLength >4) { 
				printf("RECORDED, this is win %i\n",numWins); 
				 winStarts[numWins][0] = x;  
				 winStarts[numWins][1] = y; 
				printf("checking: (%i, %i) %i, %i\n", x,y,winStarts[numWins][0], winStarts[numWins][1]);
				 numWins++;
				
			}
		} else printf("(%i, %i), Skipped\n");
				
	}

	for (int win = 0; win< numWins; win++)
	{
		printf("Win at %i, %i\n", winStarts[win][0], winStarts[win][1]);

		//TODO NEXT Delete the wins. Could try to do this as they are found, or at least mark them as they are found. Then delete the marks. Easy way is just to search from the coordinates given in the win list and delete all the marked items

	}

	printBoard(clearBoard);
	
}


int createGame(int (*gameBoard)[NUMCOLS], int seed)
{
	int row,col =0;

	Rng tileRng;
	tileRng.setSeed(seed);

	for (row = 0; row<NUMROWS; row++)
	{
		for (col = 0; col<NUMCOLS; col++)
		{
			tileRng.generateNext();


			gameBoard[row][col] = 1+ tileRng.getNext(NUMSYMBOLS);

		}
	}

}

int main()
{
	int gameBoard[NUMROWS][NUMCOLS] = {0};

	createGame(gameBoard, 1);
	printBoard(gameBoard);
	checkGame(gameBoard);



}