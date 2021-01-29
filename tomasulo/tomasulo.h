#ifndef TOMASULO
#define	TOMASULO
#include "com.h"
#include "def.h"

#define MININT	0X3F3F3F3F

/*define reservation stations*/
struct RS{
	int index;		//the index of reservation station
	bool busy;		//whether the reservation station is busy
	bool running; //whether the operator begins to run (the two sources both OK)
	int time;
	int op;			//operator 
	double	source1;	//source 1
	int source1_tag;//the tag of source 1
	double source2;	//source 2 
	int source2_tag;//the tag of source 2
	double res;		//the result
	RS() {
		index = -1;
		source1_tag = source2_tag = MININT;
		busy = running = time = op = source1 = source2 = res = 0;
	}
};
	
/*define float-point registers(FLRs)*/
struct FLR{
	int		index;		//the index of FLR
	bool	busy;		//whether the FLR is busy
	int		tag;		//if the data is not ready, write its tag;
	double	data;		//data
	FLR() {
		index = -1;
		tag = MININT;
		busy = data = 0;
	}
};

/*store data buffers(SDBs)*/
struct SDB{
	int		time;
	bool	running;
	int		index;		//the index of SDB
	bool	busy;
	int		tag;		//if the data is not ready, write its tag;
	int		addr; 
	double	data;		//data
	SDB() {
		index = -1;
		tag = MININT;
		time = running = addr = busy = data = 0;
	}
};

/*float-point buffers(FLBs)*/
struct FLB {
	int		time;
	bool	running;
	int		index;		//the index of SDB
	bool	busy;		////whether the FLB is busy
	int		addr;		//if the data is not ready, write its addr;
	double	data;		//data
	FLB() {
		index = -1;
		time = running = busy = addr = data = 0;
	}
};


void Tomasulo(int &inst_index);

#endif // !TOMASULO
