#include <stdio.h>
#include "C:/MinGW/include/randomc/randomc.h"
#include "C:/MinGW/include/randomc/mersenne.cpp"
#include <time.h>   

using namespace std;

 int seed = (int)time(0); 
	 CRandomMersenne RanGen(seed); 

int main()
{
	const int NUMTRIALS = 1000000;
	int jarFillRate[6] = {5,4,4,3,3,2};
	int jarCapacity[6] = {5,5,5,5,5,5};
	int jarContents[6] = {0,0,0,0,0,0};
	int rng = -1;

	int numJarsExploded = 0;
	int stats[7] = {0};

	for (int trial = 0; trial<NUMTRIALS; trial++)
	{
	 	numJarsExploded = 0;
	 	
	 	for (int jar=0; jar<6; jar++) 
	 	{
	 		rng = RanGen.IRandom(0,jarFillRate[jar]-1); //try to fill the jar
	 		//printf("J%i:%2i", jar, rng);
	 		if (rng == 0) // if you do fill it
	 		{
		 		jarContents[jar]++;
		 		if (jarContents[jar] >= jarCapacity[jar]) //check if it was full
		 		{
		 			jarContents[jar] = 0;
		 			numJarsExploded++;
		 			//printf("*");
		 		} //else printf(" ");

		 	}//else printf(" ");
	 	}
	 	stats[numJarsExploded]++;
	 	//if (numJarsExploded>0) printf(" [%i Explodes]", numJarsExploded);
	 //printf("--\n");
	}

	printf("Explodes: \n");
	for (int exp=0; exp<7; exp++) 
	{
		printf("%i : %i \n", exp, stats[exp]);
	}

	printf("Jars: \n");
	for (int jar=0; jar<6; jar++) 
	{
		printf("Jar %i : %i \n", jar, jarContents[jar]);
	}
	 	

}