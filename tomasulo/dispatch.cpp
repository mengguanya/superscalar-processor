#include"dispatch.h"

using namespace std;

extern int inst[INSTNUM][4];
extern int inst_num;
extern FLOS flos[FLOSNUM];
extern int flos_num;

/*simply the design of dispatch*/
void Dispatch() {
	for (int i = 0; i < inst_num; i++) {
		if (inst[i][0] != 0) {
			flos[i].inst[0] = inst[i][0];
			flos[i].inst[1] = inst[i][1];
			flos[i].inst[2] = inst[i][2];
			flos[i].inst[3] = inst[i][3];
		}
	}
}