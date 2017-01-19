#include <time.h>   
#include <math.h>
#include <stdio.h>   

#define NUMROWS 5
#define NUMCOLS 5
#define NUMSYMBOLS 6
#define MINCLUSTERLENGTH 3

#define STATE_BOUGHTGAME 100
#define STATE_FREEGAME 200


#include "jamjarsPaytable.h"

class Rng {
	unsigned long long initialSeed;
	unsigned long long nextSeed;

	public:
		void setSeed(int seed) {initialSeed = seed; nextSeed = seed;}
	unsigned long long generateNext() { 
		int skip = -1;
		if (nextSeed == 1) skip = 1; else skip = 0;

		for (int times = 0; times<1+skip; times++) nextSeed = ( nextSeed * 654188429 ) % 899809343;}
	//	unsigned long long generateNext() { nextSeed = ( nextSeed * 5 ) % 11;}
		
		unsigned long long getNext() {  return nextSeed;}
		int getNext(int mod) { return nextSeed % mod;}
		int getSeed() { return initialSeed;}



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
	markBoard[x][y] = gameBoard[x][y]; //mark a square once you check it

	int found = 1;
	int xdelta = 0;
	int ydelta = 0;

	int deltalist[4][2] = 
	{
		{-1, 0}, //up
		{1, 0}, //down
		{0, -1}, //left
		{0, 1}  //right
	};

	for (int delta = 0; delta<4; delta++)
	{	
		xdelta = deltalist[delta][0];
		ydelta = deltalist[delta][1];
		 

		// printf("checking %i, %i\n", x+xdelta, y+ydelta);

		 if (y+ydelta >= 0 && x+xdelta >= 0 && y+ydelta < NUMCOLS && x+xdelta < NUMROWS){   //check that where you want to go is in bounds
		 	if (markBoard[x+xdelta][y+ydelta] == 0) //check you haven't been there before
		 	{
		 		//printf("I found %i\n", gameBoard[x+xdelta][y+ydelta]);
		 		if (gameBoard[x+xdelta][y+ydelta] == gameBoard[x][y]) {  //if the symbol matches
		 			//printf ("That's a match, so I'll see if there are any more\n");
		 			found += checkSquare(gameBoard, markBoard, x+xdelta, y+ydelta);  //go investigate that square to find more and tell me how many you found
		 		}
		 		

		 	} //else printf("..............already been there\n");
		 } //else printf("..............past edge\n");


	}

	return found;
}

int deleteAndPopulate(int (*gameBoard)[NUMCOLS], int (*clearBoard)[NUMCOLS], Rng *tileRng )
{
	int spotsToFill[NUMCOLS] = {0};


	int row = 0;
	int col =0;

	for (row = 0; row<5;  row++)
	{
		col=0;
		spotsToFill[row] = 0;

		for (col = 0; col < NUMCOLS; col++) {

			if (clearBoard[row][col] == 0)  
			{
				gameBoard[row][col-spotsToFill[row]] = gameBoard[row][col];
			}	else spotsToFill[row]++;

		}

		
		for (col = NUMCOLS-1; col > NUMCOLS-spotsToFill[row] -1; col--) 
		{	
			tileRng->generateNext();
			gameBoard[row][col] = 1+ tileRng->getNext(NUMSYMBOLS); //TODO NEXT - Populate
		}

	}
	printf ("%i %i %i %i %i ",spotsToFill[0], spotsToFill[1], spotsToFill[2], spotsToFill[3], spotsToFill[4]);

	printf(">>\n");





	return 0;
}

int applyFeature( int (*gameBoard)[NUMCOLS], int featureType)
{
	printf("FEATURE CALL FOR JAM %i \n", featureType);
}

int checkGame(int state, int (*gameBoard)[NUMCOLS], int *jam, int *triggers, Rng *tileRng)
{
	int markBoard[NUMROWS][NUMCOLS] = {{0}};
	int clearBoard[NUMROWS][NUMCOLS] = {{0}};

	int clusterLength = 1;

	int winStarts[NUMCOLS*NUMROWS][2] = {{0}};
	int numWins = 0;

	int wins = 0;

	//Apply any special features
	if (state == STATE_FREEGAME)
	{
		if (triggers[0] > 0)
		{
			for (int t=1; t<7; t++) {
				if (triggers[t] >0) applyFeature(  gameBoard, t);
				triggers[t] = 0;
			}

			triggers[0] = 0;
		}
	}

	//Check all squares
	for (int x=0; x<NUMCOLS; x++)
	for (int y=0; y<NUMCOLS; y++)
	{
		if (markBoard[x][y] == 0)
		{
			clusterLength = checkSquare(gameBoard, markBoard, x, y);
			printf("(%i, %i), Symbol = %i, Number =  %i\n", x, y, gameBoard[x][y],clusterLength);
			if (clusterLength >= MINCLUSTERLENGTH) { 
				jam[ gameBoard[x][y]  ] += jamCollection[gameBoard[x][y]][clusterLength];  //award jam based on the cluser
				if ( jam[ gameBoard[x][y] ] >= jamTriggers[ gameBoard[x][y] ] ) { triggers[ gameBoard[x][y] ] ++; jam[ gameBoard[x][y]] = 0; triggers[0]++;} //mark triggers as ready if the jam jar is full
				printf("RECORDED, this is win %i\n",numWins); 
				 winStarts[numWins][0] = x;  
				 winStarts[numWins][1] = y; 
				printf("checking: (%i, %i) %i, %i\n", x,y,winStarts[numWins][0], winStarts[numWins][1]);
				 numWins++;
				 wins += paytable[gameBoard[x][y]][0];
				 printf ("PAY OF %i \n", paytable[gameBoard[x][y]][0]);

				
			}
		} else printf("(%i, %i), Skipped\n");  //don't check the square if it was already looked at
				
	}



	//Mark winning combinations for deletion
	for (int win = 0; win< numWins; win++)
	{
		printf("Win at %i, %i\n", winStarts[win][0], winStarts[win][1]);

		checkSquare(gameBoard, clearBoard, winStarts[win][0], winStarts[win][1]);
	}

	printBoard(clearBoard);

	deleteAndPopulate(gameBoard, clearBoard, tileRng);

	printBoard(gameBoard);

	return wins;
	
}




int createGame(int (*gameBoard)[NUMCOLS], Rng *tileRng)
{
	int row,col =0;
	printf("Seed: %i\n", tileRng->getSeed());
	int tile = 0;
	int rngResult = 0;

	for (row = 0; row<NUMROWS; row++)
	{
		for (col = 0; col<NUMCOLS; col++)
		{
			tileRng->generateNext();
			rngResult = tileRng->getNext();
			tile = 1+ tileRng->getNext(NUMSYMBOLS);

			gameBoard[row][col] = tile;
			//printf("%i  > %i.\n", rngResult, tile);

		}
	}

}

int shuffleBoard(int (*gameBoard)[NUMCOLS], Rng *tileRng )
{
	int numShuffles = 0;

	tileRng->generateNext();
	numShuffles = 15 + tileRng->getNext(15);
	int temp =0;

	printf ("Going to do %i shuffles\n", numShuffles);

	int sA, sB, rA,cA,rB,cB = 0;

	for (int shuffle = 0; shuffle< numShuffles; shuffle++)
	{
		tileRng->generateNext();
		sA = tileRng->getNext(NUMROWS*NUMCOLS);

		rA = sA % NUMROWS;
		cA = (sA - (sA % NUMROWS) ) / NUMCOLS;

		//printf("%i is %i, %i\n", sA, rA, cA);

		tileRng->generateNext();
		sB = tileRng->getNext(NUMROWS*NUMCOLS);

		rB = sB % NUMROWS;
		cB = (sB - (sB % NUMROWS) ) / NUMCOLS;

		//printf("%i is %i, %i\n", sB, rB, cB);

		//printf ("Swap those\n");

		temp = gameBoard[rA][cA];
		gameBoard[rA][cA] = gameBoard[rB][cB];
		gameBoard[rB][cB] = temp;
	}

	printBoard(gameBoard);

}

int main()
{
	int gameBoard[NUMROWS][NUMCOLS] = {0};
	Rng tileRng;
	tileRng.setSeed(10);
	int numWins = 0;

	int jam[7] = {0};
	int jamTriggerSwitch[7] = {0};
//I int jamTriggers[7] = {0, 20, 20, 20, 20, 20, 20};

	createGame(gameBoard, &tileRng );
	printBoard(gameBoard);

	shuffleBoard(gameBoard, &tileRng);

	int state = STATE_BOUGHTGAME;

	struct gameInfo
	{
		int wins;
		int drops;
	} data;


	data.wins = 0;
	data.drops = 0;

	numWins=0; //DEBUG
	while (numWins>0)
	{
		numWins = checkGame(STATE_BOUGHTGAME, gameBoard, jam, jamTriggerSwitch, &tileRng);
		printf ("%i wins\n", numWins);
		for (int i=1; i<7; i++) printf ("%i, ", jam[i]);
				printf("\n");
	
		for (int i=1; i<7; i++) printf ("%i, ", jamTriggerSwitch[i]);
		printf("\n");
		data.wins += numWins;
		data.drops ++;
	}



	printf ("End of game. Won %i in %i drops\n", data.wins, data.drops);

	if (jamTriggerSwitch[0] > 10000)
	{
		printf("%i Jam Jars ready to trigger for the free spin \n", jamTriggerSwitch[0]);
		numWins=1;

		createGame(gameBoard, &tileRng );
		printBoard(gameBoard);

		while (numWins>0)
		{
			numWins = checkGame(STATE_FREEGAME, gameBoard, jam, jamTriggerSwitch, &tileRng);
			printf ("%i wins\n", numWins);
			for (int i=1; i<7; i++) printf ("%i, ", jam[i]);
					printf("\n");
		
			for (int i=1; i<7; i++) printf ("%i, ", jamTriggerSwitch[i]);
			printf("\n");
						
		}


	}


}