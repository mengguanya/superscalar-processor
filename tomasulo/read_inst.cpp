#define _CRT_SECURE_NO_WARNINGS

#include"read_inst.h"

using namespace std;

extern FLOS flos[FLOSNUM];

int inst[INSTNUM][4];
int inst_num;
void ReadInst() {
	FILE* f = fopen("./inst.txt", "r");
	char op_t[10];
	while (~fscanf(f, "%s %d %d %d", op_t, &inst[inst_num][1], &inst[inst_num][2], &inst[inst_num][3])) {
		
		if (strcmp(op_t, "fadd.s") == 0) {
			inst[inst_num][0] = FADDS;
		}
		else if (strcmp(op_t, "fsub.s") == 0) {
			inst[inst_num][0] = FSUBS;
		}
		else if (strcmp(op_t, "flw") == 0) {
			inst[inst_num][0] = FLW;
		}
		else if (strcmp(op_t, "fsw") == 0) {
			inst[inst_num][0] = FSW;
		}
		else if (strcmp(op_t, "fmul.s") == 0) {
			inst[inst_num][0] = FMULS;
		}
		else if (strcmp(op_t, "fdiv.s") == 0) {
			inst[inst_num][0] = FDIVS;
		}
		else
		{
			inst[inst_num][0] = NOP;
		}
		inst_num++;
	}
}

