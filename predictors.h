/*
 * constants.h
 *
 *  Created on: Apr 7, 2012
 *      Author: munawirakotyad
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

typedef unsigned long ulong;


enum
{
	NOT_TAKEN =0,
			TAKEN
};

enum
{
	STRONGLY_NTAKEN=0,
			WEAKLY_NTAKEN,
			WEAKLY_TAKEN,
			STRONGLY_TAKEN
};

enum
{
	BIMODAL=0,
			GSHARE,
			HYBRID
};

struct bimodal_table
{
	ulong indexVal;
	int prediction;

};

struct gshare_table
{
	ulong indexVal;
	int prediction;

};

struct hybrid_table
{
	ulong indexVal;
	int chooserCounter;
};

struct bimodal_table *biTable;
struct gshare_table *gTable;
struct hybrid_table *hTable;

int MODE;
int K,M1,N,M2;
ulong bimodalIndex=0;
ulong gshareIndex=0;
ulong hybridIndex=0;
int gsharePrediction = 0;
int bimodalPrediction=0;
ulong numPredictions =0;
ulong numMissPredictions=0;
float missRate=0;
int bhRegister =0;
int bimodalSize;
int gshareSize;
int hybridSize;

#endif /* CONSTANTS_H_ */
