#include"com.h"
#include"tomasulo.h"

int		reg[REGNUM];		//int reg
double		mem[MEMSIZE];		//memory	
FLR		flr[FLRNUM];		//float-reg
RS		fadd_rs[FADDRSNUM];	// add/sub reservatin station
RS		fmul_rs[FADDRSNUM];	// mult/div reservatin station
FLOS	flos[FLOSNUM];		//float-point operand stack
SDB		sdb[SDBNUM];		//store data buffers
FLB		flb[FLBNUM];		//float-point buffers

int		cycle = 0;			//周期数

/*定义tomasulo部件数量*/
extern int flr_num;
extern int fmul_rs_num;
extern int fadd_rs_num;
extern int flos_num;
extern int sdb_num;
extern int flb_num;

/*定义tomasulo延迟*/
extern int fadds_latency;
extern int fsubs_latency;
extern int fmuls_latency;
extern int fdivs_latency;
extern int flw_latency;
extern int fsw_latency;

/*display all info
* int		reg[REGNUM];		//int reg
* int		mem[MEMSIZE];		//memory
* FLR		flr[FLRNUM];		//float-reg
* RS		fadd_rs[FADDRSNUM];	// add/sub reservatin station
* RS		fmul_rs[FADDRSNUM];	// mult/div reservatin station
* FLOS	flos[FLOSNUM];		//float-point operand stack
* SDB		sdb[SDBNUM];		//store data buffers
* FLB		flb[FLBNUM];		//float-point buffers
*/

string DisplayOP(int op) {
	switch (op)
	{
	case FADDS:
		return   "fadds ";
		break;
	case FSUBS:
		return  "fsubs ";
		break;
	case FMULS:
		return  "fmuls ";
		break;
	case FDIVS:
		return  "fdivs ";
		break;
	case FLW:
		return  "flw ";
		break;
	case FSW:
		return "fsw ";
		break;
	case NOP:
		return " ";
		break;
	default:
		break;
	}
}

void Display() {
	cout << "cycle: " << cycle << endl;
	cout << "index\tinst\t\tissue\tcomplete\tcommit" << endl;
	for (int i = 0; i < flos_num; i++) {
		if (flos[i].issue != 0) {
			cout << flos[i].index << "\t";
			cout << DisplayOP(flos[i].inst[0]);
			cout << flos[i].inst[1] << " " << flos[i].inst[2] << " " << flos[i].inst[3] << "\t" << flos[i].issue << "\t" << flos[i].complete << "\t" << flos[i].commit << endl;
		}
	}
	cout << endl;
	cout << "add reserved station" << endl;
	cout << "time\tbusy\trunning\top\tindex\tsource1_tag\tsource1\tsource2_tag\tsource2" << endl;
	for (int i = 0; i < fadd_rs_num; i++) {
		cout << fadd_rs[i].time << "\t" << fadd_rs[i].busy << "\t" << fadd_rs[i].running << "\t" << DisplayOP(fadd_rs[i].op) << "\t" << fadd_rs[i].index << "\t" << fadd_rs[i].source1_tag << "\t" << fadd_rs[i].source1 << "\t" << fadd_rs[i].source2_tag << "\t" << fadd_rs[i].source2 << endl;
	}
	cout << endl;
	cout << "mul reserved station" << endl;
	cout << "time\tbusy\trunning\top\tindex\tsource1_tag\tsource1\tsource2_tag\tsource2" << endl;
	for (int i = 0; i < fmul_rs_num; i++) {
		cout << fmul_rs[i].time << "\t" << fmul_rs[i].busy << "\t" << fmul_rs[i].running << "\t" << DisplayOP(fmul_rs[i].op)  << "\t" << fmul_rs[i].index  << "\t" << fmul_rs[i].source1_tag << "\t" << fmul_rs[i].source1 << "\t" << fmul_rs[i].source2_tag << "\t" << fmul_rs[i].source2 << endl;
	}

	cout << endl;
	cout << "float-point buffer" << endl;
	cout << "time\tbusy\trunning\taddr\tdata" << endl;
	for (int i = 0; i < flb_num; i++) {
		cout << flb[i].time <<"\t"<< flb[i].busy << "\t" << flb[i].running << "\t" << flb[i].addr << "\t" << flb[i].data << endl;
	}
	cout << endl;
	cout << "store buffer" << endl;
	cout << "time busy running addr tag data" << endl;
	for (int i = 0; i < sdb_num; i++) {
		cout << sdb[i].time << "\t" << sdb[i].busy << "\t" << sdb[i].running << "\t" << sdb[i].addr << "\t" << sdb[i].tag << "\t" << sdb[i].data << endl;
	}

	cout << endl;
	cout << "float-point registers" << endl;
	cout << "busy\tindex\ttag\tdata" << endl;
	for (int i = 0; i < flr_num; i++) {
		if (flr[i].busy) {
			flr[i].index = i;
			cout << flr[i].busy << "\t" << flr[i].index << "\t" << flr[i].tag << "\t" << flr[i].data << endl;
		}
	}
}


int CheckAddRS() {
	for (int i = 0; i < FADDRSNUM; i++) {
		if (!fadd_rs[i].busy) {
			return i;
		}
	}
	return -1;
}

int CheckMulRS() {
	for (int i = 0; i < FMULRSNUM; i++) {
		if (!fmul_rs[i].busy) {
			return i;
		}
	}
	return -1;
}

int CheckSDB() {
	for (int i = 0; i < SDBNUM; i++) {
		if (!sdb[i].busy) {
			return i;
		}
	}
	return -1;
}

int CheckFLB() {
	for (int i = 0; i < FLBNUM; i++) {
		if (!flb[i].busy) {
			return i;
		}
	}
	return -1;
}

bool CheckFLR(int index) {
	return flr[index].busy;
}

bool CheckREG(int index) {
	return true;
}

/*
* 指令发射到保留站和store buffer等处并不一定会立即执行（run），需要通过tag标志判断数据是否准备好，
* 若数据准备好了，则可以开始执行
* 由于保留站和store buffer的tag数量不同，分别写他们的JudgeRun函数
*/
void JudgeAddRSRun(int k) {
	if ((fadd_rs[k].source1_tag == -1) && (fadd_rs[k].source2_tag == -1)) {
		fadd_rs[k].running = true;
		if (fadd_rs[k].op == FADDS) {
			fadd_rs[k].time = fadds_latency;
		}
		else if (fadd_rs[k].op == FSUBS) {
			fadd_rs[k].time = fsubs_latency;
		}
	}
}

void JudgeMulRSRun(int k) {
	if ((fmul_rs[k].source1_tag == -1) && (fmul_rs[k].source2_tag == -1)) {
		fmul_rs[k].running = true;
		if (fmul_rs[k].op == FMULS) {
			fmul_rs[k].time = fmuls_latency;
		}
		else if (fmul_rs[k].op == FDIVS) {
			fmul_rs[k].time = fdivs_latency;
		}
	}
}

void JudgeSDBRun(int k) {
	if ((sdb[k].tag == -1)) {
		sdb[k].running = true;
		sdb[k].time = fsw_latency;
	}
}

/*
* CDB function simulates the common data bus
* after the arithmetic and load operator finished, broadcast the tag(index) and result on the common data bus
* This simulator run this step in the next cycle of arithmetic operator finished.
*/
void CDB(int index, double res, int fadd_rs_num, int fmul_rs_num, int sdb_num) {
	
	
	for (int i = 0; i < flr_num; i++) {
		if (flr[i].tag == index) {
			flr[i].busy = 0;
			flr[i].data = res;
			flr[i].tag = -1;
		}
	}

	for (int i = 0; i < fadd_rs_num; i++) {
		if (fadd_rs[i].source1_tag == index) {
			fadd_rs[i].source1_tag = -1;
			fadd_rs[i].source1 = res;
			JudgeAddRSRun(i);
		}
		if (fadd_rs[i].source2_tag == index) {
			fadd_rs[i].source2_tag = -1;
			fadd_rs[i].source2 = res;
			JudgeAddRSRun(i);
		}
	}

	for (int i = 0; i < fmul_rs_num; i++) {
		if (fmul_rs[i].source1_tag == index) {
			fmul_rs[i].source1_tag = -1;
			fmul_rs[i].source1 = res;
			JudgeMulRSRun(i);
		}
		if (fmul_rs[i].source2_tag == index) {
			fmul_rs[i].source2_tag = -1;
			fmul_rs[i].source2 = res;
			JudgeMulRSRun(i);
		}
	}

	for (int i = 0; i < sdb_num; i++) {
		if (sdb[i].tag == index) {
			sdb[i].tag = -1;
			sdb[i].data = res;
			JudgeSDBRun(i);
		}
	}
}

void Tomasulo(int &inst_index) {
	/*定义本周期已经发射的指令数目和发射宽度*/
	int issue_num = 0;
	int issue_width = ISSUEWIDTH;
	
	/*每次执行完成一个指令（time减少为0），向common data bus发送信号，这些信号需要收集起来，在本周期最后统一发出（因为模拟器是串行工作，如果立即发出信号的话，会被同周期的指令接收到）*/
	vector<vector<int>> broadcast_signal;

	/*更新周期数*/
	cycle++;
	
	/*****************************执行指令**************************************/
/*执行加减法指令*/
	for (int i = 0; i < fadd_rs_num; i++) {
		int op = fadd_rs[i].op;

		if (fadd_rs[i].busy) {
			if (!fadd_rs[i].running) {
				if ((fadd_rs[i].source1_tag == -1) && (fadd_rs[i].source2_tag == -1)) {
					fadd_rs[i].running = true;
					if (op == FADDS) {
						fadd_rs[i].time = fadds_latency;
					}
					else if (op == FSUBS) {
						fadd_rs[i].time = fsubs_latency;
					}
				}
			}
			else {
				if (fadd_rs[i].time != 0) {
					fadd_rs[i].time--;
					if (fadd_rs[i].time == 0) {
						double data1, data2;
						data1 = fadd_rs[i].source1;
						data2 = fadd_rs[i].source2;
						fadd_rs[i].res = (op == FADDS) ? (data1 + data2) : (data1 - data2);

						flos[fadd_rs[i].index].complete = cycle;
					}
				}
				else {
					int index;
					index = fadd_rs[i].index;
					flos[index].commit = cycle;
					
					vector<int> signal_t;
					signal_t.push_back(index);
					signal_t.push_back(fadd_rs[i].res);
					broadcast_signal.push_back(signal_t);

					fadd_rs[i].busy = 0;
					fadd_rs[i].running = 0;
					fadd_rs[i].source1_tag = 0;
					fadd_rs[i].source2_tag = 0;
				}
			}
		}
	}

	/*执行乘除法指令*/
	for (int i = 0; i < fmul_rs_num; i++) {
		int op = fmul_rs[i].op;

		if (fmul_rs[i].busy) {
			if (!fmul_rs[i].running) {
				if ((fmul_rs[i].source1_tag == -1) && (fmul_rs[i].source2_tag == -1)) {
					fmul_rs[i].running = true;
					if (op == FMULS) {
						fmul_rs[i].time = fmuls_latency;
					}
					else if (op == FDIVS) {
						fmul_rs[i].time = fdivs_latency;
					}
				}
			}
			else {
				if (fmul_rs[i].time != 0) {
					fmul_rs[i].time--;
					if (fmul_rs[i].time == 0) {
						double data1, data2;
						op = fmul_rs[i].index;
						data1 = fmul_rs[i].source1;
						data2 = fmul_rs[i].source2;
						fmul_rs[i].res = (op == FMULS) ? (data1 * data2) : (data1 / data2);

						flos[fmul_rs[i].index].complete = cycle;
					}
				}
				else {
					int index;
					index = fmul_rs[i].index;

					flos[index].commit = cycle;
					
					/*收集广播信号*/
					vector<int> signal_t;
					signal_t.push_back(index);
					signal_t.push_back(fmul_rs[i].res);
					broadcast_signal.push_back(signal_t);

					fmul_rs[i].busy = 0;
					fmul_rs[i].running = 0;
					fmul_rs[i].source1_tag = 0;
					fmul_rs[i].source2_tag = 0;
				}
			}
		}
	}

	/*执行load指令*/
	for (int i = 0; i < flb_num; i++) {
		int op = fadd_rs[i].op;

		if (flb[i].busy) {
			if (flb[i].running) {
				if (flb[i].time != 0) {
					flb[i].time--;
					if (flb[i].time == 0) {
						flb[i].data = mem[flb[i].addr];
						flos[flb[i].index].complete = cycle;
					}
				}
				else {
					int index;
					index = flb[i].index;

					flos[index].commit = cycle;

					/*收集广播信号*/
					vector<int> signal_t;
					signal_t.push_back(index);
					signal_t.push_back(flb[i].data);
					broadcast_signal.push_back(signal_t);

					flb[i].busy = 0;
				}
			}
		}
	}

	/*执行store指令*/
	for (int i = 0; i < sdb_num; i++) {
		int op = fadd_rs[i].op;

		if (sdb[i].busy) {
			if (!sdb[i].running) {
				if (sdb[i].tag != -1) {
					sdb[i].running = true;
					sdb[i].time = fsw_latency;
				}
			}
			else {
				if (sdb[i].time != 0) {
					sdb[i].time--;
					if (sdb[i].time == 0) {
						mem[sdb[i].addr] = sdb[i].data;

						flos[sdb[i].index].complete = cycle;
					}
				}
				else {
					int index;
					index = sdb[i].index;

					flos[index].commit = cycle;

					sdb[i].busy = 0;
				}
			}
		}
	}

	for (int i = 0; i < broadcast_signal.size(); i++) {
		CDB(broadcast_signal[i][0], broadcast_signal[i][1], fadd_rs_num, fmul_rs_num, sdb_num);
	}
	while (!broadcast_signal.empty())
	{
		broadcast_signal.pop_back();
	}
	/*****************************发射指令**************************************/

	/*指令的标号不能超过浮点寄存器堆的数量，并且本周期发射指令数目不能大于指令宽度*/
	while (inst_index < flos_num && issue_num < issue_width) {

		/*记录指令类型*/
		int op = flos[inst_index].inst[0];

		/*如果是加减法指令*/
		if ((op == FADDS) || (op == FSUBS)) {
			/*加减法指令发射条件：加减法保留站有空间*/
			int add_rs_index = CheckAddRS();

			/*加减法指令发射条件不满足*/
			if (add_rs_index == -1) {
				break;
			}
			
			/*加减法指令发射条件满足，发射加减法指令到add保留站，并记录发射时间*/
			fadd_rs[add_rs_index].op = op;
			fadd_rs[add_rs_index].busy = true;
			fadd_rs[add_rs_index].index = inst_index;

			flos[inst_index].issue = cycle;

			int d = flos[inst_index].inst[1];
			/*把浮点寄存器里标记为busy，并记下flr.tag = index*/
			flr[d].busy = 1;
			flr[d].tag = fadd_rs[add_rs_index].index;


			int s1 = flos[inst_index].inst[2];
			int s2 = flos[inst_index].inst[3];
			/*如果源浮点寄存器数据准备好了，则读取数据，否则读取它的tag*/
			if (!CheckFLR(s1)) {
				fadd_rs[add_rs_index].source1 = flr[s1].data;
				fadd_rs[add_rs_index].source1_tag = -1;
			}
			else {
				fadd_rs[add_rs_index].source1_tag = flr[s1].tag;
			}
			if (!CheckFLR(s2)) {
				fadd_rs[add_rs_index].source2 = flr[s2].data;
				fadd_rs[add_rs_index].source2_tag = -1;
			}
			else {
				fadd_rs[add_rs_index].source2_tag = flr[s2].tag;
			}
			JudgeAddRSRun(add_rs_index);
			
			issue_num++;
			inst_index++;
		}
		/*如果是乘除法指令*/
		else if ((op == FMULS) || (op == FDIVS)) {
			/*乘除法指令发射条件：乘除法保留站有空间*/
			int mul_rs_index = CheckMulRS();

			/*乘除法指令发射条件不满足*/
			if (mul_rs_index == -1) {
				break;
			}

			/*乘除法指令发射条件满足，发射乘除法指令到mul保留站，并记录发射时间*/
			fmul_rs[mul_rs_index].op = op;
			fmul_rs[mul_rs_index].busy = true;
			fmul_rs[mul_rs_index].index = inst_index;

			flos[inst_index].issue = cycle;

			int d = flos[inst_index].inst[1];
			/*把浮点寄存器里标记为busy，并记下flr.tag = index*/
			flr[d].busy = 1;
			flr[d].tag = fmul_rs[mul_rs_index].index;

			int s1 = flos[inst_index].inst[2];
			int s2 = flos[inst_index].inst[3];

			/*如果源浮点寄存器数据准备好了，则读取数据，否则读取它的tag*/
			if (!CheckFLR(s1)) {
				fmul_rs[mul_rs_index].source1 = flr[s1].data;
				fmul_rs[mul_rs_index].source1_tag = -1;
			}
			else {
				fmul_rs[mul_rs_index].source1_tag = flr[s1].tag;
			}
			if (!CheckFLR(s2)) {
				fmul_rs[mul_rs_index].source2 = flr[s2].data;
				fmul_rs[mul_rs_index].source2_tag = -1;
			}
			else {
				fmul_rs[mul_rs_index].source2_tag = flr[s2].tag;
			}

			JudgeMulRSRun(mul_rs_index);

			issue_num++;
			inst_index++;
		}

		else if (op == FSW) {

			/*检查是否满足发射条件：sdb有空间，并且store指令的源地址寄存器数据准备好*/
			int sdb_index = CheckSDB();
			int d = flos[inst_index].inst[1];
			int s1 = flos[inst_index].inst[2];
			int s2 = flos[inst_index].inst[3];

			/*发射条件不满足，不发射*/
			if ((sdb_index == -1) || !CheckREG(s1)) {
				break;
			}

			/*发射条件满足，发射这条store指令到sdb，并记录发射时间*/
			int data1 = reg[s1];
			int data2 = s2;
			sdb[sdb_index].addr = data1 + data2;
			sdb[sdb_index].busy = true;

			flos[inst_index].issue = cycle;

			/*如果store的浮点寄存器的数据没有准备好，则在sdb里记录flr的标志*/
			if (flr[d].busy) {
				sdb[sdb_index].tag = flr[d].tag;
			}
			else {
				sdb[sdb_index].data = flr[d].data;
				sdb[sdb_index].tag = -1;
			}
			JudgeSDBRun(sdb_index);

			issue_num++;
			inst_index++;
		}
		else if (op == FLW) {
			/*检查发射条件是否满足：flb有空间，并且load指令的源地址寄存器数据准备好*/
			int flb_index = CheckFLB();

			int d = flos[inst_index].inst[1];
			int s1 = flos[inst_index].inst[2];
			int s2 = flos[inst_index].inst[3];

			/*发射条件不满足，不发射*/
			if ((flb_index == -1) || !(CheckREG(s1))) {
				break;
			}

			/*发射条件满足，发射这条load指令到flb，并记录发射时间*/
			flb[flb_index].busy = true;
			flb[flb_index].index = inst_index;

			flos[inst_index].issue = cycle;

			/*开始执行load指令，并开启倒计时*/
			flb[flb_index].running = true;

			int data1 = reg[s1];
			int data2 = s2;
			flb[flb_index].addr = data1 + data2;
			flb[flb_index].time = flw_latency;

			/*把目的浮点寄存器标志为busy，并记录下索引*/
			flr[d].busy = true;
			flr[d].tag = flb[flb_index].index;

			issue_num++;
			inst_index++;
		}
		else {
			break;
		}
	}

	Display();
}
