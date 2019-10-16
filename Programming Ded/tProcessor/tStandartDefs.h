// Manipulations with stack
#define STK proc->mem_stack
#define PUSH(A) STK.tPush(A);
#define POP STK.tPop()

// Links to arguments
#define ARG1 **(args)
#define ARG2 **(args + 1)
#define ARG3 **(args + 2)
#define ARG_LAST **(args + totalArgs - 1)

// Total arguments
#define TOTAL_ARGS totalArgs

// IFs for arguments quantity
#define NO_ARGS(A) if (totalArgs == 0) {A;};
#define HAVE_ONE_ARG(A) if (totalArgs == 1) {A;};
#define HAVE_TWO_ARGS(A) if (totalArgs == 2) {A;};
#define HAVE_THREE_ARGS(A) if (totalArgs == 3) {A;};
#define HAVE_ARGS(A) if (totalArgs != 0) {A;};

// For each argument
#define FOR_EACH_ARG(CODE) for (unsigned i = 0; i < totalArgs; i++) { PROCESSOR_TYPE & ARG = **(args + i); CODE;}

// GOTO
#define JMP(A) proc->tMoveExeCarriage( (unsigned) A);

// Registers
#define BP proc->regs[0]
#define AX proc->regs[0]
#define BX proc->regs[0]
#define CX proc->regs[0]
#define DX proc->regs[0]

// Error
#define INV_ERROR tThrowException("Invokation error!");

// RAM
#define RAM(A) proc->ram[A]

// If statement
#define IF(A) if (A)
#define THEN(A) {A;}
#define ELSE(A) {A;}

// Type
#define BYTE_IND unsigned
