#define STK proc->mem_stack
#define PUSH(A) STK.tPush(A);
#define POP STK.tPop()
#define ARG *arg
#define HAVE_ARG(A) if (arg != NULL) {A;};
#define NO_ARG(A) if (arg == NULL) {A;};
