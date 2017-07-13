#include <time.h>   
#include <math.h>
#include <stdio.h>   

#define NUMROWS 7
#define NUMCOLS 7
#define NUMSYMBOLS 6
#define MINCLUSTERLENGTH 3

#define STATE_BOUGHTGAME 0
#define FEATURE_INSEASON 1
#define FEATURE_FRUITBURST 2
#define FEATURE_FRUITBUNCH 3


#include "jamjarsPaytable.h"

class Rng {
	unsigned long long initialSeed;
	unsigned long long nextSeed;

	public:
		void setSeed(int seed) {initialSeed = seed; nextSeed = seed;}
	
		//get the next random number and store it in the classes property
		unsigned long long generateNext() { 
			int skip = -1;
			if (nextSeed == 1) skip = 1; else skip = 0;

			for (int times = 0; times<1+skip; times++) nextSeed = ( nextSeed * 654188429 ) % 899809343;
		}
		
		unsigned long long getNext() {  return nextSeed;} //return the next seed as in
		int getNext(int mod) { return nextSeed % mod;} //return the next seed with a modulo on it. 
		int getSeed() { return initialSeed;} //return the initial seed

} ;


/*
	print the game board to help with debugging. 
*/
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


/*
	Finds clusters beggining at a given square:
	Check a square to see if any of its neighbours match. Then see if that has any matching neighbours.
	It will mark off a path of connected symbols if it exists. 

	gameBoard: what you are checking
	markBoard: where you'd like to make the marks
	x, y: postion to begin at 
*/

int checkSquare(int (*gameBoard)[NUMCOLS], int (*markBoard)[NUMCOLS], int x, int y)
{
	//printf("SQUAURE %i, %i\n", x, y);
	markBoard[x][y] = gameBoard[x][y]; //mark a square once you investigate it

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

	//Check up, down, left and right
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
		 		} //if it doesn't match what we are looking for, no need to investigate ,but it might be part of another win, so don't mark it off, we want to maybe visit it later
		 		

		 	} //else printf("..............already been there\n");
		 } //else printf("..............past edge\n");


	}

	return found;
}

/*
	Deletes any symbols marked, drops the remaining symbols, then fills it with new ones

*/
int deleteAndPopulate(int (*gameBoard)[NUMCOLS], int (*clearBoard)[NUMCOLS], Rng *tileRng )
{
	int spotsToFill[NUMCOLS] = {0};


	int row = 0;
	int col =0;

	for (row = 0; row<NUMROWS;  row++)
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
			gameBoard[row][col] = 1+ tileRng->getNext(NUMSYMBOLS); 
		}

	}
	//printf ("%i %i %i %i %i ",spotsToFill[0], spotsToFill[1], spotsToFill[2], spotsToFill[3], spotsToFill[4]);

	//printf(">>\n");





	return 0;
}

//Makes it eay to go horizontal and vertial and diagonal, just say what way you want to go and what number you want in the sequence)
int gridLoc( int *vec, int i, int j)
{

	if (i==0) { vec[0] = j; vec[1] = j; } //TL-BR
	if (i==1) { vec[0] = 6-j; vec[1] = j; } //BL-TR
	if (i==2) { vec[0] = 3; vec[1] = j; } //HOR
	if (i==3) { vec[0] = j; vec[1] = 3; } //VERT


}

int applyFeature( int (*gameBoard)[NUMCOLS], int featureType, int featureFruit, Rng *tileRng )
{
	printf("FEATURE CALL. FEATURE %i FOR JAM %i \n", featureType, featureFruit);
	int clearBoard[NUMROWS][NUMCOLS] = {{0}};

	if (featureType == FEATURE_INSEASON)
	{
			int x = 0;
			int y = 0;
			int winMade = 0;
			int safety = 0;

			printf("Here is where you started:\n");
			printBoard(gameBoard);

			while (!winMade && safety<100)
			{
				tileRng->generateNext();
				x = tileRng->getNext(7); 
				tileRng->generateNext();
				y = tileRng->getNext(7); 

				if (gameBoard[x][y] != featureFruit)
				{ 
					gameBoard[x][y] = featureFruit;
					printf("Added a fruit at %i,%i\n", x, y);
					printBoard(gameBoard);
					winMade = checkSquare(gameBoard, clearBoard, x, y);
					safety++;
					printf("How many wins? %i\n", winMade);
					if (winMade < 4) {
						winMade=0;
						for (int i=0; i<7; i++) for (int j=0; j<7; j++) clearBoard[i][j] = 0;
					}
				} else
				{
					safety++;
					printf("no need to check, this position alrady contains the fruit\n");
				}
			} if (safety>40) printf("ERROR- COULD NOT FIND WIN IN INSEASON FEATURE\n");
	}
	else if (featureType == FEATURE_FRUITBUNCH)
	{
			int numBunches = 0;
			int rngResult	=0;
			int bunch[4] = {0};
			//TO DO - ADD WEIGHTS
 
 			//bunches listed at their x,4. 2x2 then goes tl-br
			int bunchstart[4][2] =    
			{
					{1,1},
					{4,1},
					{1,4},
					{4,4}
			};
				
			//where to move
			int bunchOffset[4][2] = 
			{
				{0,0},
				{0,1},
				{1,0},
				{1,1}
			};

			while (numBunches	== 0)
			{
					for (int b = 0; b<4; b++)
					{
							tileRng->generateNext();
							rngResult = tileRng->getNext(10);  

							for (int s=0; s<4; s++)
							{

									if (rngResult<5)    //50-50 chance, can change this
									{
										//delete
										clearBoard[bunchstart[b][0]+bunchOffset[s][0] ][bunchstart[b][1]+bunchOffset[s][1] ] = 1;

									}
									else
									{
										//keep
										gameBoard[bunchstart[b][0]+bunchOffset[s][0] ][bunchstart[b][1]+bunchOffset[s][1] ] = featureFruit	;


									}
							}
					}
				numBunches--;
			}

			printBoard(clearBoard);
			deleteAndPopulate(gameBoard, clearBoard, tileRng);
			printBoard(gameBoard);


	}
	else if (featureType == FEATURE_FRUITBURST)
	{
		int numFruits = 0;
		int numExplosions = 0;
		int numMystery = 0;
		int fruitsOnBoard = 0;
		int rngResult = 0;


		int position[2] = {0,0};
		int *pos = position;

		//See how many fruits there are
		for (int direction = 0; direction<4; direction++)
		{
			for (int i=0; i<7; i++){
				gridLoc(pos, direction, i);
				if ( gameBoard[position[0]][position[1]] == featureFruit) fruitsOnBoard	++;	
			}
		}

		printf("I found %i \n", fruitsOnBoard);

		numFruits = 8 - (int)((float)fruitsOnBoard/2.5);  //Arbitrary, can change if want more of one or the other
		numExplosions = 8 - (int)((float)fruitsOnBoard/3.5); //Arbitrary, can change if want more of one or the other
		numMystery = 25 - numFruits - numExplosions - fruitsOnBoard;
		printf("%2i: %2i added, %2i explosions, %2i mystery\n", fruitsOnBoard, numFruits, numExplosions, numMystery);
	
		//Assign the mysteries
		while (numMystery > 0)
		{
			tileRng->generateNext();
			rngResult = tileRng->getNext(10);
			if (rngResult<8) rngResult	= 0; else rngResult	 = 1;  // 50-50  spit, can change this, arbitrary, can change if want more of one or the other
			if (rngResult == 0) numFruits++; else numExplosions++;

			numMystery--;
		}
		printf("%2i: %2i added, %2i explosions, %2i mystery\n", fruitsOnBoard, numFruits, numExplosions, numMystery);

		//Go through the burst and put in all the new symbols and explosions

		//TL - BR diagonal
		for (int direction = 0; direction<4; direction++)
		{
								for (int i=0; i<7; i++) 
								{
									gridLoc(pos, direction, i);
									printf("I am at %i, %i. %i is here ", position[0],position[1], gameBoard[position[0]][position[1]]	);

									if (gameBoard[position[0]][position[1]] != featureFruit)
									{
										
										if (   (position[0]==3 && position[1]==3 && direction==0)  || !(position[0] ==3  && position	[1] ==3)         )
										{

										tileRng->generateNext();
										rngResult = tileRng->getNext(2);  // 50-50  spit, can change this, arbitrary, can change if want more of one or the other
										printf(" so I need to do something. What do I do? %i ", rngResult);
										if (rngResult == 0 && numFruits) 
										{	

											// pick a fruit
											numFruits--;
											gameBoard[position[0]][position[1]]	= featureFruit; 
											printf(" pick a fruit!\n");
										} else
										{
											//pick a mystery
											numMystery--;
											gameBoard[position[0]][position[1]]	= 9;
											clearBoard[position[0]][position[1]] = 1;
											printf(" blow shit up\n");
										}
										} else printf("Ive already been to the centre\n");
									}
									else printf(" so I don't do anything here\n");
								}
								printf("\n");
		}

		printBoard	(clearBoard	);
		printBoard	(gameBoard	);
		deleteAndPopulate(gameBoard, clearBoard, tileRng);
		
  // 		//BL - TR diagonal
		// for (int i=0; i<7; i++) if (gameBoard[6-i][i] == featureFruit) fruitsOnBoard++;
		
		// //horizontal
		// for (int i=0; i<7; i++) if (gameBoard[3][i] == featureFruit) fruitsOnBoard++;
			
		// //vertical
		// for (int i=0; i<7; i++) if (gameBoard[i][3] == featureFruit) fruitsOnBoard++;
		// //delete and populate


	}
	else
	{
		printf("!-ERROR-! : Invalid Feature\n");
	}

			printf("----------------------------END OF FEATURE--------------------\n");
			printBoard	(clearBoard	);
			printBoard	(gameBoard	);
				
}


/*
	Check the array for wins
*/
int checkGame(int state, int (*gameBoard)[NUMCOLS], int *jam, int *triggers, Rng *tileRng)
{
	int markBoard[NUMROWS][NUMCOLS] = {{0}}; //this is so you know where you have looked
	int clearBoard[NUMROWS][NUMCOLS] = {{0}}; //this is so you know what to delete

	int clusterLength = 1;

	int winStarts[NUMCOLS*NUMROWS][2] = {{0}}; //This keeps a track of where the wins start from, so we can find them later.
	int numWins = 0; 

	int wins = 0;

	// //Apply any special features
	// if (state != STATE_BOUGHTGAME)
	// {
	// 		for (int t=1; t<7; t++) {
	// 			//printf("Trigger %i : %i\n", t, triggers[t]);
	// 			if (triggers[t] >0) {
	// 				applyFeature(  gameBoard, state, t, tileRng);
	// 				triggers[t]--;
	// 				break;
	// 				}
				
	// 		}
		
	// }

	//Check all squares
	for (int x=0; x<NUMCOLS; x++)
	for (int y=0; y<NUMCOLS; y++)
	{
		if (markBoard[x][y] == 0) //only look into it if we haven't been there before
		{
			clusterLength = checkSquare(gameBoard, markBoard, x, y); 
			printf("(%i, %i), Symbol = %i, Number =  %i\n", x, y, gameBoard[x][y],clusterLength);
			if (clusterLength >= MINCLUSTERLENGTH) { //only care if it's a win, and it can only be a win if it's bigger than the minimum cluster length
				jam[ gameBoard[x][y]  ] += jamCollection[gameBoard[x][y]][clusterLength];  //award jam based on the cluser
				if ( jam[ gameBoard[x][y] ] >= jamTriggers[ gameBoard[x][y] ] ) { triggers[ gameBoard[x][y] ] ++; jam[ gameBoard[x][y]] = 0; triggers[0]++;} //mark triggers as ready if the jam jar is full
				printf("RECORDED, this is win %i\n",numWins); 
				 winStarts[numWins][0] = x;   //record the win
				 winStarts[numWins][1] = y; 
				printf("checking: (%i, %i) %i, %i\n", x,y,winStarts[numWins][0], winStarts[numWins][1]);
				 numWins++; 
				 wins += paytable[gameBoard[x][y]][clusterLength]; //award the win
				 printf ("PAY OF %i \n", paytable[gameBoard[x][y]][clusterLength]);

				
			}
		} else printf("(%i, %i), Skipped\n");  //don't check the square if it was already looked at
				
	}

	//Mark winning combinations for deletion
	for (int win = 0; win< numWins; win++)
	{
		printf("Win at %i, %i\n", winStarts[win][0], winStarts[win][1]);

		checkSquare(gameBoard, clearBoard, winStarts[win][0], winStarts[win][1]);
	}

	printf("Ready to delete\n");
	printBoard(clearBoard);
	deleteAndPopulate(gameBoard, clearBoard, tileRng);
	printf("New board:\n");
	printBoard(gameBoard);

	return wins;
	
}


/*
	create a fresh game
*/

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
	int jamCheck = 0;
	int featureChoice = 0;

	createGame(gameBoard, &tileRng );
	printBoard(gameBoard);


	int state = STATE_BOUGHTGAME;

	struct gameInfo
	{
		int wins;
		int drops;
		int totalWins;
		int totalDrops;
	} data;


	data.wins = 0;
	data.drops = 0;
	data.totalWins = 0;
	data.totalDrops = 0;

	int gameOver = 0;

	jam[2] +=100;
	
	while (!gameOver)
	{
	numWins=-1; 
	while (numWins>0 || numWins == -1)
	{
		if (numWins == -1) numWins = 0;
		numWins = checkGame(STATE_BOUGHTGAME, gameBoard, jam, jamTriggerSwitch, &tileRng);
		printf ("%i wins\n", numWins);
		for (int i=0; i<7; i++) printf ("%i, ", jam[i]);
				printf("\n");
	
		for (int i=0; i<7; i++) printf ("%i, ", jamTriggerSwitch[i]);
		printf("\n");
		data.wins += numWins;
		data.drops ++;
		if (data.drops > 100) numWins = 0;

	}

	data.totalWins += data.wins;
	data.totalDrops += data.drops;

	printf ("...................................End of this set of drops. Won %i in %i drops\n", data.wins, data.drops);

	data.wins = 0;
	data.drops = 0;
	
	jamCheck = 6;
	featureChoice = FEATURE_FRUITBURST; //TO DO - ADD SELECTION


	if (jamTriggerSwitch[0] > 0)
	{
		printf("There was a jam trigger!\n");
		while (jamCheck>0)
		{
			printf("Was it %i? \n", jamCheck);
			if (jamTriggerSwitch[jamCheck] > 0) 
			{
				jamTriggerSwitch[jamCheck]--;

				applyFeature(  gameBoard, featureChoice, jamCheck, &tileRng);

				jamCheck = 0; 

			} else	jamCheck--;
		}

		jamTriggerSwitch[0]--;
	} else gameOver = 1;



	}
	//numWins = checkGame(FEATURE_INSEASON, gameBoard, jam, jamTriggerSwitch, &tileRng);

	//need to keep going, but remember that you can't call the feature again or it will do its thing, you need to check the rest of the game as if its a normal game after this. 

printf ("...................................End of game. Won %i in %i drops\n", data.totalWins, data.totalDrops);



}