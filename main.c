#include <stdbool.h>
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
  INSTRUCTION_ADD,
  INSTRUCTION_DUMP,
  INSTRUCTION_HALT,
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

  bool halted;
} Stacky;

#ifdef TEST_MODE
void test_main(void);
#endif
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

void stacky_instruction_add(Stacky *stacky) {
  int64_t operand_b = stacky_pop_value(stacky);
  int64_t operand_a = stacky_pop_value(stacky);
  int64_t result = operand_a + operand_b;

  stacky_push_value(stacky, result);
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
    case INSTRUCTION_ADD: {
      assert(instruction.argument == 0);
      stacky_instruction_add(stacky);
      break;
    }
    case INSTRUCTION_DUMP: {
      stacky_dump_value(stacky);
      break;
    }
    case INSTRUCTION_HALT: {
      stacky->halted = true;
      break;
    }
  }
}

/*
 * Description: Push a new instruction into the code segement of the stacky machine
 * @stacky: the virutal machine where to push the instruction
 * @instruction: the instruction to push
 */
void stacky_push_code_instruction(Stacky *stacky, StackyInstruction instruction) {
  assert(stacky != NULL);
  assert(stacky->code_segment_size < CODE_SEGMENT_MAX_CAPACITY);

  stacky->code_segment[stacky->code_segment_size++] = instruction;
}

/*
 * Description: Generate a new instruction 
 * @opcode: the operation of the instruction itself
 * @argument: optional argument, should be set to zero if no arguments are necessary for this operation
 * returns: the generated instruction
 */
StackyInstruction emitter_generate_instruction(StackInstructionType opcode, int64_t argument) {
  StackyInstruction result = {
    .opcode = opcode,
    .argument = argument
  };

  return result;
}

int main(void) {
  Stacky stacky_vm = {0};

  #ifdef TEST_MODE
    test_main();
  #else
  //TODO(Kay): Add Make_Instruction Function
  stacky_push_code_instruction(&stacky_vm, emitter_generate_instruction(INSTRUCTION_PUSH, 1));
  stacky_push_code_instruction(&stacky_vm, emitter_generate_instruction(INSTRUCTION_PUSH, 1));
  stacky_push_code_instruction(&stacky_vm, emitter_generate_instruction(INSTRUCTION_ADD, 0));
  stacky_push_code_instruction(&stacky_vm, emitter_generate_instruction(INSTRUCTION_DUMP, 0));

  for(size_t i = 0; i < stacky_vm.code_segment_size && !stacky_vm.halted; i++) {
    stacky_execute_cycle(&stacky_vm);
  }
  #endif

  return 0;
}


#ifdef TEST_MODE
void test_check_push_instruction(void) {
  Stacky stacky_vm = {0};

  stacky_push_code_instruction(&stacky_vm, emitter_generate_instruction(INSTRUCTION_PUSH, 1));
  stacky_push_code_instruction(&stacky_vm, emitter_generate_instruction(INSTRUCTION_HALT, 0));
  
  for(size_t i = 0; i <stacky_vm.code_segment_size && !stacky_vm.halted; i++) {
    stacky_execute_cycle(&stacky_vm);
  }

  int64_t expected = 1;
  int64_t result = stacky_pop_value(&stacky_vm);

  assert(stacky_vm.halted == true);
  assert(result == expected);
}

void test_check_result_addition(void) {
  Stacky stacky_vm = {0};

  stacky_push_code_instruction(&stacky_vm, emitter_generate_instruction(INSTRUCTION_PUSH, 1));
  stacky_push_code_instruction(&stacky_vm, emitter_generate_instruction(INSTRUCTION_PUSH, 1));
  stacky_push_code_instruction(&stacky_vm, emitter_generate_instruction(INSTRUCTION_ADD, 0));
  stacky_push_code_instruction(&stacky_vm, emitter_generate_instruction(INSTRUCTION_HALT, 0));

  int64_t expected = 2;

  for(size_t i = 0; i < stacky_vm.code_segment_size; i++) {
    stacky_execute_cycle(&stacky_vm);
  }

  int64_t result = stacky_pop_value(&stacky_vm);

  assert(stacky_vm.halted == true);
  assert(result == expected);
}

void test_main(void) {
  test_check_push_instruction();
  test_check_result_addition();
}
#endif
