/*
Paytable.

Format:
{
	JAM0 - Pay for OAK 0, 1, 2, 3, 4 ... 15
	JAM1 - Pay for OAK 0, 1, 2, 3, 4 ... 15
	...
	JAM6 - Pay for OAK 0, 1, 2, 3, 4 ... 15
}
All pays listed as multipliers. IE 15 means 15 x bet

Remember that some pays can be the same, ie, 12 OAK, 13 OAK and 14 OAK might be the same. This is how the paytable is, if you look on games like Gemix, the paytable says things like 12-14 

*/
const int paytable[8][16] = 
{
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0,0,0,0,0,350,700,1000,1500,2500,4000,4000,20000,20000,20000,100000},
	{0,0,0,0,0,250,350,500,750,1000,1500,1500,10000,10000,10000,50000},
	{0,0,0,0,0,100,200,300,500,750,1500,1500,5000,5000,5000,20000},
	{0,0,0,0,0,75,100,150,200,300,500,500,1000,1000,1000,5000},
	{0,0,0,0,0,30,40,50,75,100,200,200,500,500,500,1500},
	{0,0,0,0,0,15,25,30,50,75,100,100,250,250,250,1000},
};


/*
Jam Collection Rate 

{
	JAM0 - Amout of Jam collected for OAK 0, 1, 2, 3, 4 ... 15
	JAM1 - Amout of Jam collected for OAK 0, 1, 2, 3, 4 ... 15
	...
	JAM6 - Amout of Jam collected for OAK 0, 1, 2, 3, 4 ... 15
}

Let's call them, drops. Ie, 5 of a kind JAM1 gives x drops of jam.

*/
const int jamCollection[8][16]=
{
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 0, 0, 0, 0, 0, 0},
	{20, 21, 22, 23, 24, 25, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{60, 61, 62, 63, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

};

/*
Jam Triggers (should be called Jam Capacity).

Dertemines when the jars are full. IE, When JARN has X drops, it is full

{JAM0_CAPACITY, JAM1_CAPACITY ... JAM6_CAPACITY}

*/
const int jamTriggers[7] = {0, 120, 120, 120, 120, 120, 120};

/*
Bucket Weights.

Determines which bucket to choose when running the simulator.

Notes:
- The format is in the "Weighted" format: {number_of_items, total_weight, item1_weight, item2_weight ... itemN_weight}
- The layout is just to make it easier to compare / send to and from Excel it could be in a straight line, but now it's:

NUMBER_OF_BUCKETS, TOTAL_WEIGHT_OF_BUCKETS,

BUCKET0,
BUCKET1, BUCKET2, 	... BUCKET5,
BUCKET6, ...		...	BUCKET9,
...,
BUCKET21, ...		...	BUCKET25,


*/
int bucketWeights[28] = 
{
26,13,

10,
1,0,0,0,0,
1,0,0,0,0,
1,0,0,0,0,
0,0,0,0,0,
0,0,0,0,0,
};

/*
Bucket Size.

Specifies the size of the buckets, ie, how many seeds are in each bucket. 
The format is the same as the second part of the bucket weights, just there to make it easier to copy to/from Excel

BUCKET0
BUCKET1 BUCKET2 ... BUCKET5
BUCKET6 ...		...	BUCKET9
...
BUCKET21 ...	...	BUCKET25

*/
const int bucketSize[26] = 
{
4,
3,0,0,0,0,
2,0,0,0,0,
1,0,0,0,0,
0,0,0,0,0,
0,0,0,0,0,


};

/*
Bucket File Size.

??

*/
const int bucketFileSize[26] = 
{
0,
0,0,0,0,0,
0,0,1,1,1,
0,1,1,0,1,
0,0,0,0,1,
0,0,1,0,1,
};

/*
Pattern Weights.

Determines which JAMSLOT to put into the grid when one is needed.

In the Weighted format: {NUMBER_OF_JAMSLOTS, TOTAL_WEIGHT, JAMSLOT1_WEIGHT, ... JAMSLOT6 WEIGHT}

*/
 int patternWeights[8] = {6,6,1,1,1,1,1,1};


/*
Jam Weights.

Determines which JAMSLOT to put into the grid when one is needed.

In the Weighted format: {NUMBER_OF_JAMS, TOTAL_WEIGHT, JAM1_WEIGHT, ... JAM6 WEIGHT}

*/
const int jamWeights[8] = {6, 21,1,3,6,4,5,2};

