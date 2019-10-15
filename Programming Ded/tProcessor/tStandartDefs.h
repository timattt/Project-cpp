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
