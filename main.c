#include <stdio.h>

#include "lstring.h"

#define STACK_MAX_CAPACITY 256
#define CODE_SEGMENT_MAX_CAPACITY 1024

typedef enum {
  STACKY_STACK_OVERFLOW,
  STACKY_STACK_UNDERFLOW,
} StackyErrorStates;

typedef enum {
  INSTRUCTION_PUSH,
  INSTRUCTION_POP,
  INSTRUCTION_DUMP,
} StackInstructionType;

typedef struct {
  StackInstructionType opcode;
  int64_t argument; 
} StackyInstruction;

typedef struct {
  size_t stack_pointer;
  int64_t stack[STACK_MAX_CAPACITY];

  size_t instruction_pointer;
  size_t code_segment_size;
  StackyInstruction code_segment[CODE_SEGMENT_MAX_CAPACITY];
} Stacky;

/*
 * Description: Push a new value onto the stack, crash if we cannot safely fit the value onto the stack.
 * @stacky: The virtual machine.
 * @value: The value to push
*/
void stacky_push_value(Stacky *stacky, int64_t value) {
  assert(stacky != NULL);

  if(stacky->stack_pointer >= STACK_MAX_CAPACITY) {
    fprintf(stderr, "stacky_vm error: cannot push value, maximum stack capacity reached!\n");
    exit(STACKY_STACK_OVERFLOW);
  }

  stacky->stack[stacky->stack_pointer++] = value;
}

/*
* Description: Pop a value fromt the stack, crash if we cannot safely pop a value without risking an underflow.
* @stacky: The virtual machine.
* Returns: the popped value
*/
int64_t stacky_pop_value(Stacky *stacky) {
  assert(stacky != NULL);

  if(stacky->stack_pointer < 1) {
    fprintf(stderr, "stacky_vm error: cannot pop value, minimal safe stack position reached\n");
    exit(STACKY_STACK_UNDERFLOW);
  }

  return stacky->stack[--stacky->stack_pointer];
}

void stacky_dump_value(Stacky *stacky) {
  assert(stacky != NULL);

  int64_t value = stacky->stack[stacky->stack_pointer - 1];
  fprintf(stdout, "DUMP [%ld]\n", value);
}

/*
 * Description: Execute the next instruction that the instruction_pointer looks at.
 * @stacky - The virtual machine.
 */
void stacky_execute_cycle(Stacky *stacky) {
  //TODO(Kay): The instruction_pointer++ might bite us in the ass later when there are relative jumps from the current
  //           position. We should keep that in mind!
  StackyInstruction instruction = stacky->code_segment[stacky->instruction_pointer++]; 

  switch(instruction.opcode) {
    case INSTRUCTION_PUSH: {
      stacky_push_value(stacky, instruction.argument);
      break;
    } 
    case INSTRUCTION_POP: {
      stacky_pop_value(stacky);
      break;
    }
    case INSTRUCTION_DUMP: {
      stacky_dump_value(stacky);
      break;
    }
  }
}

void stacky_push_code_instruction(Stacky *stacky, StackyInstruction instruction) {
  assert(stacky != NULL);
  assert(stacky->code_segment_size < CODE_SEGMENT_MAX_CAPACITY);

  stacky->code_segment[stacky->code_segment_size++] = instruction;
}

int main(void) {
  Stacky stacky_vm = {0};

  //TODO(Kay): Add Make_Instruction Function
  stacky_push_code_instruction(&stacky_vm, (StackyInstruction) { .opcode = INSTRUCTION_PUSH, .argument = 1 });
  stacky_push_code_instruction(&stacky_vm, (StackyInstruction) { .opcode = INSTRUCTION_DUMP, .argument = 0 });

  stacky_execute_cycle(&stacky_vm);
  stacky_execute_cycle(&stacky_vm);

  return 0;
}
