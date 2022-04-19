/*
 * branchPredictor.cc
 *
 *  Created on: Apr 7, 2012
 *      Author: munawirakotyad
 */


#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iomanip>
using namespace std;

#include "predictors.h"

void generateIndex(ulong programCounter, int MODE);
void predictOutcome(int MODE, char *prediction);
void updateBHT(int MODE, char actual);

void printOutput(int MODE);
int main(int argc, char *argv[])
{
	int i;
	FILE *trace;
	char actual;
	char prediction;
	char PC[20];

	ulong programCounter;

	K=M1=N=M2=0;

	if(argc < 2)
	{
		cout << "Number of input parameters are not sufficient\n";
		return 0;
	}

	cout << "COMMAND\n";

	if(strcmp(argv[1],"bimodal")== 0)
	{
		if(argc < 4)
		{
			cout << "Number of input parameters are not sufficient\n";
			return 0;
		}

		M2 = atoi(argv[2]);
		trace = fopen(argv[3], "r");
		MODE = BIMODAL;
		bimodalSize = pow(2,M2);
		biTable = (bimodal_table *)malloc(bimodalSize*sizeof(bimodal_table));

		for(i=0;i<bimodalSize;i++)
		{
			biTable[i].prediction = WEAKLY_TAKEN;
			biTable[i].indexVal = i;

		}
		cout << "./sim bimodal" <<" "<< M2 <<argv[3]<< endl;

	}


	if(strcmp(argv[1],"gshare")== 0)
	{
		if(argc < 4)
		{
			cout << "Number of input parameters are not sufficient\n";
			return 0;
		}

		M1 = atoi(argv[2]);
		N = atoi(argv[3]);
		trace = fopen(argv[4],"r");
		MODE = GSHARE;
		gshareSize = pow(2,M1);
		gTable = (gshare_table*)malloc(gshareSize*sizeof(gshare_table));
		//BHR = (int *)malloc(N*sizeof(int));
		for(i=0;i<gshareSize;i++)
		{
			gTable[i].prediction = WEAKLY_TAKEN;
			gTable[i].indexVal = i;

		}
		cout << "./sim  gshare " <<" "<< M1 <<" "<< N <<" "<< argv[4]<< endl;

	}

	if(strcmp(argv[1],"hybrid")== 0)
	{
		if(argc < 7)
		{
			cout << "Number of input parameters are not sufficient\n";
			return 0;
		}

		K = atoi(argv[2]);
		M1 = atoi(argv[3]);
		N = atoi(argv[4]);
		M2 = atoi(argv[5]);
		trace = fopen(argv[6],"r");
		MODE = HYBRID;

		bimodalSize = pow(2,M2);
		biTable = (bimodal_table *)malloc(bimodalSize*sizeof(bimodal_table));
		gshareSize = pow(2,M1);
		gTable = (gshare_table*)malloc(gshareSize*sizeof(gshare_table));

		hybridSize = pow(2,K);
		hTable = (hybrid_table*)malloc(hybridSize * sizeof(hybrid_table));

		for(i=0;i<gshareSize;i++)
		{
			gTable[i].prediction = WEAKLY_TAKEN;
			gTable[i].indexVal = i;

		}

		for(i=0;i<bimodalSize;i++)
		{
			biTable[i].prediction = WEAKLY_TAKEN;
			biTable[i].indexVal = i;

		}

		for(i=0;i<hybridSize;i++)
		{
			hTable[i].chooserCounter = WEAKLY_NTAKEN;
			hTable[i].indexVal =i;
		}
		cout << "./sim  hybrid " << " "<< K <<" "<< M1 <<" "<< N <<" "<< M2<<" "<< argv[5]<< endl;

	}


	while(fscanf(trace, "%lx %c", &programCounter,&actual)>0)
	{
		numPredictions++;
		sprintf(PC, "%lx", programCounter);

		generateIndex(programCounter,MODE);

		predictOutcome(MODE,&prediction);

		if(prediction != actual)
			numMissPredictions++;

		updateBHT(MODE,actual);


	}

	double mRate;
	mRate = ((double)numMissPredictions/(double)numPredictions);
	cout << "OUTPUT\n";

	cout<< "number of predictions:\t "<< numPredictions <<"\n";
	cout<< "number of mispredictions:\t " << numMissPredictions <<"\n";
	printf("misprediction rate:\t %5.2f ",(mRate*100));
	printf("%\n");


	printOutput(MODE);

	fclose(trace);

	if(hTable!=NULL)
		free(hTable);
	if(biTable != NULL)
		free(biTable);
	if(gTable!=NULL)
		free(gTable);

	exit(0);
}


void generateIndex(ulong programCounter, int MODE)
{
	programCounter >>= 2;
	ulong temp =0;
	ulong index =0;
	ulong bhrMask =0;
	switch (MODE)
	{

	case BIMODAL:

		temp = pow(2,M2)-1 ;
		bimodalIndex = programCounter & temp;


		break;

	case GSHARE:
		temp = pow(2,M1)-1;
		temp = programCounter&temp;

		if(M1 != N)
		temp = temp >> (M1-N);

		gshareIndex = bhRegister^temp;

		if(M1 != N)
		{
			gshareIndex = gshareIndex << (M1 -N);
			temp = programCounter << (64-(M1-N));
			temp >>= (64-(M1-N));
			gshareIndex = gshareIndex | temp;
		}


		break;

	case HYBRID:
		temp = pow(2,M2)-1 ;
		bimodalIndex = programCounter & temp;

		temp = pow(2,M1)-1;
		temp = programCounter&temp;
		temp = temp >> (M1-N);
		gshareIndex = bhRegister^temp;
		gshareIndex = gshareIndex << (M1 -N);
		temp = programCounter << (64-(M1-N));
		temp >>= (64-(M1-N));
		gshareIndex = gshareIndex | temp;

		temp = pow(2,K)-1 ;
		hybridIndex = programCounter & temp;

		break;

	}

}

void predictOutcome(int MODE, char *prediction)
{

	switch (MODE)
	{

	case BIMODAL:

		if(biTable[bimodalIndex].prediction > WEAKLY_NTAKEN)
			*prediction = 't';
		else
			*prediction = 'n';
		break;

	case GSHARE:
		if(gTable[gshareIndex].prediction > WEAKLY_NTAKEN)
			*prediction = 't';
		else
			*prediction = 'n';
		break;

	case HYBRID:

		if(biTable[bimodalIndex].prediction > WEAKLY_NTAKEN)
			bimodalPrediction = 't';
		else
			bimodalPrediction = 'n';

		if(gTable[gshareIndex].prediction > WEAKLY_NTAKEN)
			gsharePrediction = 't';
		else
			gsharePrediction = 'n';

		if(hTable[hybridIndex].chooserCounter > 1)
			*prediction = gsharePrediction;
		else
			*prediction = bimodalPrediction;


		break;
	}
}

void updateBHT(int MODE, char actual)
{
	int mask;
	int mask1;
	switch (MODE)
	{
	case BIMODAL:
		if(actual == 't' && biTable[bimodalIndex].prediction < STRONGLY_TAKEN)
			biTable[bimodalIndex].prediction++;

		if(actual == 'n' && biTable[bimodalIndex].prediction > STRONGLY_NTAKEN)
			biTable[bimodalIndex].prediction--;

		break;

	case GSHARE:
		if(actual == 't' && gTable[gshareIndex].prediction < STRONGLY_TAKEN)
			gTable[gshareIndex].prediction++;

		if(actual == 'n' && gTable[gshareIndex].prediction > STRONGLY_NTAKEN)
			gTable[gshareIndex].prediction--;

		mask = pow(2,N)-1;
		bhRegister >>=1;
		bhRegister &= mask;
		mask1 =pow(2,N-1);
		if(actual == 't')
			bhRegister |= mask1;

		break;

	case HYBRID:

		if(hTable[hybridIndex].chooserCounter > 1)
		{
			if(actual == 't' && gTable[gshareIndex].prediction < STRONGLY_TAKEN)
				gTable[gshareIndex].prediction++;

			if(actual == 'n' && gTable[gshareIndex].prediction > STRONGLY_NTAKEN)
				gTable[gshareIndex].prediction--;

			mask = pow(2,N)-1;
			bhRegister >>=1;
			bhRegister &= mask;
			mask1 =pow(2,N-1);
			if(actual == 't')
				bhRegister |= mask1;
		}
		else
		{
			if(actual == 't' && biTable[bimodalIndex].prediction < STRONGLY_TAKEN)
				biTable[bimodalIndex].prediction++;

			if(actual == 'n' && biTable[bimodalIndex].prediction > STRONGLY_NTAKEN)
				biTable[bimodalIndex].prediction--;

			mask = pow(2,N)-1;
			bhRegister >>=1;
			bhRegister &= mask;
			mask1 =pow(2,N-1);
			if(actual == 't')
				bhRegister |= mask1;
		}

		if(actual == gsharePrediction && actual != bimodalPrediction)
		{
			if(hTable[hybridIndex].chooserCounter < 3)
				hTable[hybridIndex].chooserCounter++;
		}
		if(actual != gsharePrediction && actual == bimodalPrediction)
		{
			if(hTable[hybridIndex].chooserCounter > 0)
				hTable[hybridIndex].chooserCounter--;
		}




		break;
	}

}
void printOutput(int MODE)
{
	int i;

	switch (MODE)
	{
	case BIMODAL:
		cout << " FINAL BIMODAL CONTENTS \n";
		for(i=0;i<bimodalSize;i++)
			cout << biTable[i].indexVal << "\t" << biTable[i].prediction << "\n";
		break;

	case GSHARE:
		cout << " FINAL GSHARE CONTENTS \n";
		for(i=0;i<gshareSize;i++)
			cout << gTable[i].indexVal << "\t" << gTable[i].prediction << "\n";
		break;

	case HYBRID:
		cout << " FINAL CHOOSER CONTENTS \n";
		for(i=0;i<hybridSize;i++)
			cout << hTable[i].indexVal << "\t" << hTable[i].chooserCounter << "\n";
		cout << " FINAL GSHARE CONTENTS \n";
		for(i=0;i<gshareSize;i++)
			cout << gTable[i].indexVal << "\t" << gTable[i].prediction << "\n";
		cout << " FINAL BIMODAL CONTENTS \n";
		for(i=0;i<bimodalSize;i++)
			cout << biTable[i].indexVal << "\t" << biTable[i].prediction << "\n";


		break;
	}
}
