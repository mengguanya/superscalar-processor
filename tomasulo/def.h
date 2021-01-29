#ifndef DEF
#define	DEF

#define ISSUEWIDTH	1

/*define the tag of the instruction*/
#define NOP		0
#define FLW		1
#define FSW		2
#define FADDS	3
#define FSUBS	4
#define FMULS	5
#define FDIVS	6

/*define the default latency*/
#define FLWLATENCY		2
#define FSWLATENCY		2
#define	FADDSLATENCY	2
#define	FSUBSLATENCY	2
#define FMULSLATENCY	10
#define	FDIVSLATENCY	40

/*define the num of function unit*/
#define	INSTNUM		1024
#define REGNUM		32
#define	FLRNUM		32
#define	MEMSIZE		1024
#define	FADDRSNUM	3
#define	FMULRSNUM	2
#define	FLOSNUM		32
#define	SDBNUM		3	
#define	FLBNUM		3

/*define the float-point operand stack*/
struct FLOS {
	int index;		//index
	int inst[4];	//inst[0] == op; inst[1] == dest; inst[2] == source1; inst[3] == source2
	int issue;		//issue time
	int complete;	//complete time;
	int commit;		//commit time
};
#endif // !DEF

