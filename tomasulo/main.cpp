#include"main.h"

using namespace std;
/*
*	this is a tomasulo algorithm simulator, which supports configurable latency of every kind of operators.
*	This simulator supports RISC-V，including:
		lw		rd,	rs1, imm
		sw		rd, rs1, imm
		add		rd,	rs1, rs2
		addi	rd,	rs1, imm
		mul		rd, rs1, rs2
		div		rd, rs1, rs2
	operator	defalut latency
	load/store	2		
	add/sub		2
	multiply	10
	divide		40

	*********************run the main function**************************
	the simulator runs forever until you exit it
*/

/*定义tomasulo部件数量*/
int flr_num = FLRNUM;
int fmul_rs_num = FMULRSNUM;
int fadd_rs_num = FADDRSNUM;
int flos_num = FLOSNUM;
int sdb_num = SDBNUM;
int flb_num = FLBNUM;

/*定义tomasulo延迟*/
int fadds_latency = FADDSLATENCY;
int fsubs_latency = FSUBSLATENCY;
int fmuls_latency = FMULSLATENCY;
int fdivs_latency = FDIVSLATENCY;
int flw_latency = FLWLATENCY;
int fsw_latency = FSWLATENCY;

int main(int argc, char* argv[]) {

	int inst_num = 0;
	char work;
	ReadInst();
	Dispatch();
	while (true)
	{
		//cin >> work;
		Tomasulo(inst_num);
	}
}