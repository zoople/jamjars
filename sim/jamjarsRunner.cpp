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
#include "C:/random/randomc.h"
#include "C:/random/mersenne.cpp"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <dirent.h>
using namespace std;



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

struct gameInfo
	{
		int wins;
		int drops;
		int numWins;
		int totalWins;
		int totalDrops;
		int totalNumWins;
	} ;

 int seed = (int)time(0); 
	 CRandomMersenne RanGen(seed); 

int weightedDice(int* dist)
{
	int pick;

	int dice =  RanGen.IRandom(0, dist[1]-1);	
	int checkpoint=0;

	for (pick = 2; pick<=(dist[0]+1); pick++)
	{
			checkpoint += dist[pick];
			if (dice < checkpoint) break;
	}
	return pick-2;
}

int weightedDice(int* dist, Rng *tileRng)
{
	int dice = 0;
	//printf("New weighted dice\n");

	 tileRng->generateNext();
	 dice = tileRng->getNext(dist[1]);
	 dice++;

	// printf("%i, ", dice);
	return dice;
	
	

	// int checkpoint=0;

	// for (pick = 2; pick<=(dist[0]+1); pick++)
	// {
	// 		checkpoint += dist[pick];
	// 		if (dice < checkpoint) break;
	// }
	// return pick-2;
}


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
int deleteAndPopulate(int (*gameBoard)[NUMCOLS], int (*clearBoard)[NUMCOLS], Rng *tileRng, int * js)
{
	int spotsToFill[NUMCOLS] = {0};


	int row = 0;
	int col =0;
		int j;

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
			int tile = weightedDice(patternWeights, tileRng);


			if (js[0] == 0) gameBoard[row][col] = tile; 
			else {
					j = tile;
					 gameBoard[row][col] = js[j-1];

			}
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

	int jamSwitcher[6] = {0};

	if (featureType == FEATURE_INSEASON)
	{
			int x = 0;
			int y = 0;
			int winMade = 0;
			int safety = 0;

			//printf("Here is where you started:\n");
			//printBoard(gameBoard);

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
			deleteAndPopulate(gameBoard, clearBoard, tileRng, jamSwitcher	);
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

		//printBoard	(clearBoard	);
		//printBoard	(gameBoard	);
		deleteAndPopulate(gameBoard, clearBoard, tileRng, jamSwitcher	);
		
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
void checkGame(int state, int (*gameBoard)[NUMCOLS], int *jam, int *triggers, Rng *tileRng, gameInfo *info, int (*winRecorder)[16], int *jamSwitcher)
{

	int markBoard[NUMROWS][NUMCOLS] = {{0}}; //this is so you know where you have looked
	int clearBoard[NUMROWS][NUMCOLS] = {{0}}; //this is so you know what to delete

	int clusterLength = 1;

	int winStarts[NUMCOLS*NUMROWS][2] = {{0}}; //This keeps a track of where the wins start from, so we can find them later.
	int numWins = 0; 
	int wins = 0;

	//Check all squares
	for (int x=0; x<NUMCOLS; x++)
	for (int y=0; y<NUMCOLS; y++)
	{
		if (markBoard[x][y] == 0) //only look into it if we haven't been there before
		{
			clusterLength = checkSquare(gameBoard, markBoard, x, y); 
			//printf("(%i, %i), Symbol = %i, Number =  %i\n", x, y, gameBoard[x][y],clusterLength);
			if (clusterLength >= MINCLUSTERLENGTH) { //only care if it's a win, and it can only be a win if it's bigger than the minimum cluster length
				jam[ gameBoard[x][y]  ] += jamCollection[gameBoard[x][y]][clusterLength];  //award jam based on the cluser
				if ( jam[ gameBoard[x][y] ] >= jamTriggers[ gameBoard[x][y] ] ) { triggers[ gameBoard[x][y] ] ++; jam[ gameBoard[x][y]] = 0; triggers[0]++;} //mark triggers as ready if the jam jar is full
				//printf("RECORDED, this is win %i\n",numWins); 

				 winStarts[numWins][0] = x;   //record the win
				 winStarts[numWins][1] = y; 
				//printf("checking: (%i, %i) %i, %i\n", x,y,winStarts[numWins][0], winStarts[numWins][1]);
				 numWins++; 
				 wins += paytable[gameBoard[x][y]][clusterLength]; //award the win
				 printf ("PAY OF %i \n", paytable[gameBoard[x][y]][clusterLength]);
				 winRecorder[gameBoard[x][y]][clusterLength]++;
				
			}
		} //else printf("(%i, %i), Skipped\n");  //don't check the square if it was already looked at
				
	}

	//Mark winning combinations for deletion
	for (int win = 0; win< numWins; win++)
	{
		//printf("Win at %i, %i\n", winStarts[win][0], winStarts[win][1]);

		checkSquare(gameBoard, clearBoard, winStarts[win][0], winStarts[win][1]);
	}

	//printf("Ready to delete\n");
	//printBoard(clearBoard);
	deleteAndPopulate(gameBoard, clearBoard, tileRng, jamSwitcher);
	//printf("New board:\n");
	//printBoard(gameBoard);

	info->wins += wins;
	info->drops ++;
	//printf ("--------------------------------------DROP COUNTED--------------------------------------------------------------------------------\n");
	info->numWins +=numWins;
	
	return;
	
}


/*
	create a fresh game
*/

int createGame(int (*gameBoard)[NUMCOLS], Rng *tileRng)
{
	int row,col =0;
	//printf("Seed: %i\n", tileRng->getSeed());
	int tile = 0;
	int rngResult = 0;

	for (row = 0; row<NUMROWS; row++)
	{
		for (col = 0; col<NUMCOLS; col++)
		{
			
			//printf("%i(", rngResult);
			
			//printf("%i),", tile);
			tile = weightedDice(patternWeights, tileRng);
		
			//tileRng->generateNext();
			//rngResult = tileRng->getNext();
			//tile = tileRng->getNext(NUMSYMBOLS);

			
			gameBoard[row][col] = tile;
			//printf("%i  > %i.\n", rngResult, tile);

		}
		//printf("\n");
	}

}

int shuffleBoard(int (*gameBoard)[NUMCOLS], Rng *tileRng )
{
	int numShuffles = 0;

	tileRng->generateNext();
	numShuffles = 15 + tileRng->getNext(15);
	int temp =0;

	//printf ("Going to do %i shuffles\n", numShuffles);

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

	//printBoard(gameBoard);

}

void changePattern( 	int (*gameBoard)[NUMCOLS], int *jamAssignment  )
{
	//printBoard	(gameBoard	);

	//for (int i=0; i<6; i++) printf("%i, ", jamAssignment[i]);
	//	printf("\n");


	for (int x=0; x<7; x++)
			for (int y=0; y<7; y++) gameBoard[x][y] = jamAssignment[ gameBoard[x][y]-1 ];


	//printBoard	(gameBoard	);
}

void assignJam(int* whichJam)
{
	int newWeights[8] = {0};
	int rngResult = 0;


	for (int i=0; i<8; i++) newWeights[i] = jamWeights[i];

//for (int i=0; i<8; i++) printf("%i, ", newWeights[i]);
		//	printf("\n");
		
		//printf("lets begin \n");

	for (int i=0; i<6; i++)
	{
		rngResult = weightedDice(newWeights);
		//printf("I got %i\n", rngResult);
		whichJam[i] = rngResult+1;
		newWeights[1]-= newWeights[rngResult+2];
		newWeights[rngResult+2] = 0;

		//for (int i=0; i<8; i++) printf("%i, ", newWeights[i]);
		//	printf("\n");

	}


	//for (int i=0; i<6; i++) printf("%i, ", whichJam[i]);
	//printf("\n");

}

int playGame(int gameSeed, gameInfo* data, int (*winRecorder)[16])
{
	int gameBoard[NUMROWS][NUMCOLS] = {0};
	Rng tileRng;
	tileRng.setSeed(gameSeed);
	int numWins = 0;

	int whichJam[6] = {0};
	int jam[7] = {0};
	int jamTriggerSwitch[7] = {0};
	//I int jamTriggers[7] = {0, 20, 20, 20, 20, 20, 20};
	int jamCheck = 0;
	int featureChoice = 0;

	createGame(gameBoard, &tileRng );
	printBoard(gameBoard);

	//assignJam(whichJam);
	//changePattern(gameBoard, whichJam		);
	
	int state = STATE_BOUGHTGAME;

	


	data->wins=0;
	data->drops = 0;
	data->numWins = 0;
	data->totalWins = 0;
	data->totalDrops = 0;
	data->totalNumWins = 0;

	int gameOver = 0;

	jam[2] +=100;
	
	while (!gameOver)
	{
	numWins=-1; 
	while (numWins>0 || numWins == -1)
	{
		data->numWins = 0;

		if (numWins == -1) numWins = 0;
		checkGame(STATE_BOUGHTGAME, gameBoard, jam, jamTriggerSwitch, &tileRng, data, winRecorder, whichJam	);
		//printf ("%i won\n", data->wins);
		//printf ("%i total number wins\n", data->numWins);
		
		//for (int i=0; i<7; i++) printf ("%i, ", jam[i]);
		//		printf("\n");
	
		//for (int i=0; i<7; i++) printf ("%i, ", jamTriggerSwitch[i]);
		//printf("\n");
		//data.wins += numWins;
		//data.drops ++;
		//if (data->drops > 100) numWins = 0;
		//numWins = data->totalNumWins;
		numWins = data->numWins;
		data->totalNumWins += data->numWins;
	}

	data->totalWins += data->wins;
	data->totalDrops += data->drops;

	printf ("...................................End of this set of drops. Won %i in %i drops\n", data->wins, data->drops);

	data->wins = 0;
	data->drops = 0;
	
	jamCheck = 6;
	featureChoice = FEATURE_FRUITBURST; //TO DO - ADD SELECTION

	jamTriggerSwitch[0] = 0; //DEBUG - FEATURE OFF

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
	

	data->totalDrops--;


	//printf ("...................................End of game. Won %i over %i wins in %i drops\n", data->totalWins, data->totalNumWins, data->totalDrops);
	

}


int farmer(int numResults, int target)
{
	printf("Jam Jars Farmer\n");
	ofstream myfile;

	int blankRecord[8][16] = {0};

	int redrops = 1;
	int wins = 1;
	 ostringstream os;

		gameInfo data;
		int seed = -1;

	 for (int res=1; res<=numResults; res++)
	 {
	 	seed = target;
	 	printf("Farming from seed %i\n", seed);
	 	playGame(seed, &data,blankRecord);

	 	printf("To repeat, that's %i prize over %i drops with %i wins\n", data.totalWins, data.totalDrops, data.totalNumWins);

	 	redrops = data.totalDrops;
	 	wins = data.totalNumWins;

	 	if (redrops>5) redrops = 5; //LIMIT of 5, can have as many as you like...

		os << "./output/R" << redrops << "/W" << wins << ".txt";
		string filename = os.str();

		myfile.open (filename.c_str(), ios::app);

		if (!myfile.is_open()) 	 printf("not open yet\n");	else	myfile << seed <<"\n";
 		
		myfile.close();

		os.str(std::string()); //clear the stream for next time.
	}

}


// void whichWinFile(int bucketNum, int *result)
// {
// 	int drops = bucketNum/5;
// 	int wins = 

// }

int whichWinBucket(int drops, int wins)
{
	int bucketNum = -1;


	int buckets[6][5] = 
	{
		{0,0,0,0,0},			
		{1,2,3,4,5},
		{2,3,4,5,6},
		{3,4,5,6,8},
		{4,5,6,10,15},
		{5,6,7,10,15},
	};

	int bu = -1;
	for (bu=0; bu<4; bu++)
	{
		//printf("boundary is %i, and i have %i\n", buckets[drops][bu], wins);
		if (wins >= buckets[drops][bu] &&  wins < buckets[drops][bu+1]) {
			bucketNum = bu;
			break;
		}
		
	}

	if (bucketNum==-1) bucketNum = 4;

	return bucketNum;
}

int to_int(char const *s)
{
     if ( s == NULL || *s == '\0' )
        throw std::invalid_argument("null or empty string argument");

     bool negate = (s[0] == '-');
     if ( *s == '+' || *s == '-' ) 
         ++s;

     if ( *s == '\0')
        throw std::invalid_argument("sign character only.");

     int result = 0;
     while(*s)
     {
          if ( *s >= '0' && *s <= '9' )
          {
              result = result * 10  - (*s - '0');  //assume negative number
          }
          else
              throw std::invalid_argument("invalid input string");
          ++s;
     }
     return negate ? result : -result; //-result is positive!
} 

void bucketStats()
{


	int stats[26][8][16] = {{0}};

	int bucketNum = 6;
	int curBoundry = -1;
	unsigned char isFile =0x8;
	int numSeeds = 0;

	DIR *dir;
	DIR *subdir;
	
	struct dirent *ent;
	struct dirent *subent;
	
	ostringstream os;
	string filename;
	string line;
	
	int gameSeed = 0;

		gameInfo data;
	
	if ((dir = opendir ("./output")) != NULL) 
	{
	  	/* print all the files and directories within directory */
	  	while ((ent = readdir (dir)) != NULL) {
	  		//If it is a file
	    	if (ent->d_type == isFile)
	    	{
	    	 	 printf ("%s\n", ent->d_name); 
	    	} else
	    	//If it is a directory
	    	{
	    		if (ent->d_name[0] != '.') //Don't need to worry about going up directories
	    		{
		    		printf ("[%s]\n", ent->d_name);	

					os.str(std::string()); //clear the stream
					os << "./output/" << ent->d_name; //append the directory so we can read from it
					filename = os.str();
					
					numSeeds = 0;
		    		if ((subdir = opendir (filename.c_str())) != NULL) 
		    		{
		    			while ((subent = readdir (subdir)) != NULL) 
		    			{
		    				if (subent->d_name[0] != '.') 
		    				{
								printf ("%s-------------------------------------------------------------------------------\n", subent->d_name); 
								///NOW ALL THE MAGIC happens
								printf("first thing, i know that this file has %i wins and %i drops\n", subent->d_name[1]-'0', ent->d_name[1]-'0' );
								bucketNum = whichWinBucket(ent->d_name[1]-'0', subent->d_name[1]-'0');
								printf("So, ill put it in the %i bucket\n", bucketNum);
								printf("What you mean is bucket %i\n", (ent->d_name[1]-'0'-1)*5+bucketNum+1 );
								os.str(std::string()); //clear the stream
								os << "./output/" << ent->d_name;
								os << "/" << subent->d_name;
								filename = os.str();
								printf("Let's open the file: %s\n", filename.c_str());

								  ifstream myfile (filename.c_str());
								  numSeeds=0;
								  if (myfile.is_open())
								  {
								    while ( getline (myfile,line) )
								    {
								      gameSeed = to_int(line.c_str());
								      printf("Seed: %i\n", gameSeed);
								      numSeeds++;
								      playGame(gameSeed, &data, stats[(ent->d_name[1]-'0'-1)*5+bucketNum+1]);

								
								      printf ("END. Won %i over %i wins in %i drops\n", data.totalWins, data.totalNumWins, data.totalDrops);
	
								    }
								    	printf("%i seeds in this bucket\n", numSeeds);
								    	stats[(ent->d_name[1]-'0'-1)*5+bucketNum+1][0][0] = numSeeds;
								    	for (int wi=0; wi<8; wi++)
										{
											for (int wj=0; wj<15; wj++)
											{
												printf("%2i", stats[(ent->d_name[1]-'0'-1)*5+bucketNum+1][wi][wj]);
											}
											printf("\n");
										}
										

								    myfile.close();
								  }
								   	printf("Now I need to move it to the buckets folder\n");

								    	std::ifstream  src(filename.c_str(), std::ios::binary);  //already know the source

								      os.str(std::string()); //clear the stream
									  os << "./buckets/B" << ((ent->d_name[1]-'0'-1)*5+bucketNum+1);
									  os << "/";
									  os << subent->d_name;
									  filename = os.str();


									  cout << filename;
									  cout << endl;

								      std::ofstream  dst(filename.c_str(),   std::ios::binary);

								      dst << src.rdbuf();


							}
		    			}	
		    		}
		    			closedir (subdir);
		    		} else perror("");
	    		}
	    	} 
	  	  closedir (dir);
		} else {
	  	/* could not open directory */
	  	perror ("");
  	}

  	printf("*************************************************************\n");
  	for (int bucketN=0; bucketN<=25; bucketN++)
  	{
  		printf("Bucket %i------------------------------------------------\n", bucketN);
  		printf("Seeds: %i\n", stats[bucketN][0][0]);
  		for (int wi=0; wi<8; wi++)
										{
											for (int wj=0; wj<15; wj++)
											{
												printf("%2i", stats[bucketN][wi][wj]);
											}
											printf("\n");
										}




  	}


	// for (int rd=0; rd<6; rd++)
	// {
	// 	printf("REDROPS %i -----------------------------------------------------\n", rd);
	// 	for (int nwB=0; nwB<5; nwB++)
	// 	{
	// 		curBoundry = buckets[rd][nwB];
	// 		printf("Boundry: %i\n", nwB);

	// 		//As for the files that fit into the buckets
	// 		if (nwB<4) {

	// 		} 

	// 		//For anything at the end, eg, 15+, rather than asking, go through all the files and just 

	// 	}
	// }

}

int realGame()
{
	int NUMTRIALS = 500000;
	
	int bucket = -1;
	int seed = -1;
	int item = -1;
	int file =  -1;
	int fileNum = 0;
	int itemNum = 0;

	DIR *dir;
	DIR *subdir;
	
	struct dirent *ent;
	struct dirent *subent;
	int jams[6] = {0};

	ostringstream os;
	string filename;
	string line;

	gameInfo data;
	int blankRecord[8][16] = {0};	

	double average = 0.0;

	for (int trial = 0; trial < NUMTRIALS; trial++)
	{
		//Pick a buckets
		bucket = weightedDice(bucketWeights);
		//bucket = 9;
		//printf("Choosing Bucket %i \n", bucket);
			
		os.str(std::string()); //clear the stream
		os << "./buckets/B" << bucket;
		filename = os.str();
		

		if ((dir = opendir (filename.c_str())) != NULL) 
		{
			  
		} else perror("Error");
	

		if (bucketFileSize[bucket] > 1)
		{
			//printf("Ill have to pick a bucket\n");
			file = 1; //TODO
		} else file = 0;

		fileNum = 0;

		 	while ((ent = readdir (dir)) != NULL) 
		 	{
		 		if (ent->d_name[0] != '.')
		 		{
		     		//printf ("[%i] %s\n", fileNum, ent->d_name); 
		     		 if (fileNum == file) {
		//     		// 	printf("^^^this is the one \n");
		     		 	break;
		     		 } 
		//     		// else
					 fileNum++;
		     	}

		 	}

		 os << "/" << ent->d_name;
		 filename = os.str();
		 //printf("Ok, I have %s ready\n",filename.c_str());
		 //printf("Ready to choose the item now\n");

		item = RanGen.IRandom(0, bucketSize[bucket]-1);	
		//printf("Choosing Item number %i \n", item);

		 ifstream myfile (filename.c_str());

		 						 itemNum = 0;
								  if (myfile.is_open())
								  {
								    while ( getline (myfile,line) )
								    {
								      seed = to_int(line.c_str());
								      //printf("%i Seed: %i\n", itemNum, seed);
								      if (itemNum == item) break;
								      itemNum++;
								  	}
								  }


						//printf("Selected seed: %i\n", seed);

		closedir(dir);
		

		playGame(seed, &data, blankRecord);
		//printf("------------------\n");
		if (trial > 0) average = ((average*(double)trial)+(double)data.totalWins)/((double)trial+1.0) ; else average = data.totalWins;
		//printf("%10i %f\n", data.totalWins, average);
	}

	printf("Average: %f\n", average);

}






int main()
{
	//for (int i=1; i<11; i++)	farmer(1, i);
	//	//bucketStats();
	int blankRecord[8][16] = {0};
	
	gameInfo data;
	playGame(7, &data, blankRecord);
	
	



	//int gameBoard[NUMROWS][NUMCOLS] = {0};

	//Rng tileRng;
	//tileRng.setSeed(1);

	//createGame	(gameBoard, &tileRng	);

	

	//realGame();
	//playGame(1, &data);
	//printf("You belong in %i\n", data.totalDrops);

}