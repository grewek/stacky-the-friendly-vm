#include <stdbool.h>
#include <stdio.h>

#include "lstring.h"

#define STACK_MAX_CAPACITY 256
#define CODE_SEGMENT_MAX_CAPACITY 1024

typedef enum {
  STACKY_OK,
  STACKY_STACK_OVERFLOW,
  STACKY_STACK_UNDERFLOW,
} StackyErrorState;

typedef enum {
  INSTRUCTION_PUSH,
  INSTRUCTION_POP,
  INSTRUCTION_ADD,
  INSTRUCTION_SUB,
  INSTRUCTION_MUL,
  INSTRUCTION_DIV,
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
StackyErrorState stacky_push_value(Stacky *stacky, int64_t value) {
  assert(stacky != NULL);

  if(stacky->stack_pointer >= STACK_MAX_CAPACITY) {
    fprintf(stderr, "stacky_vm error: cannot push value, maximum stack capacity reached!\n");
    return STACKY_STACK_OVERFLOW;
  }

  stacky->stack[stacky->stack_pointer++] = value;

  return STACKY_OK;
}

/*
* Description: Pop a value fromt the stack, crash if we cannot safely pop a value without risking an underflow.
* @stacky: The virtual machine.
* Returns: the popped value
*/
StackyErrorState stacky_pop_value(Stacky *stacky, int64_t *value_out) {
  assert(stacky != NULL);

  if(stacky->stack_pointer < 1) {
    fprintf(stderr, "stacky_vm error: cannot pop value, minimal safe stack position reached\n");
    return STACKY_STACK_UNDERFLOW;
  }

  if(value_out != NULL) {
    *value_out = stacky->stack[--stacky->stack_pointer];
  } else {
    int64_t value = stacky->stack[--stacky->stack_pointer];
    (void)value;
  }

  return STACKY_OK;
}

StackyErrorState stacky_dump_value(Stacky *stacky) {
  assert(stacky != NULL);
  assert(stacky->stack_pointer >= 1);

  if(stacky->stack_pointer < 1) {
    fprintf(stderr, "stacky_vm error: cannot peek value when stack is empty\n");
    return STACKY_STACK_UNDERFLOW;
  }

  int64_t value = stacky->stack[stacky->stack_pointer - 1];
  fprintf(stdout, "DUMP [%ld]\n", value);

  return STACKY_OK;
}

StackyErrorState stacky_instruction_add(Stacky *stacky) {
  int64_t operand_a = 0;
  int64_t operand_b = 0;

  StackyErrorState success_b = stacky_pop_value(stacky, &operand_b);
  if(success_b != STACKY_OK) {
    return success_b;  
  }

  StackyErrorState success_a = stacky_pop_value(stacky, &operand_a);
  if(success_a != STACKY_OK) {
    return success_a;
  }

  int64_t result = operand_a + operand_b;

  return stacky_push_value(stacky, result);
}

StackyErrorState stacky_instruction_sub(Stacky *stacky) {
  int64_t operand_a = 0;
  int64_t operand_b = 0;

  StackyErrorState success_b = stacky_pop_value(stacky, &operand_b);
  if(success_b != STACKY_OK) {
    return success_b;
  }

  StackyErrorState success_a = stacky_pop_value(stacky, &operand_a);
  if(success_a != STACKY_OK) {
    return success_a;
  }

  int64_t result = operand_a - operand_b;

  return stacky_push_value(stacky, result);
}

StackyErrorState stacky_instruction_div(Stacky *stacky) {
  int64_t operand_a = 0;
  int64_t operand_b = 0;

  StackyErrorState success_b = stacky_pop_value(stacky, &operand_b);
  if(success_b != STACKY_OK) {
    return success_b;
  }

  StackyErrorState success_a = stacky_pop_value(stacky, &operand_a);
  if(success_a != STACKY_OK) {
    return success_a;
  }

  int64_t result = operand_a / operand_b;

  return stacky_push_value(stacky, result);
}

StackyErrorState stacky_instruction_mul(Stacky *stacky) {
  int64_t operand_a = 0;
  int64_t operand_b = 0;

  StackyErrorState success_b = stacky_pop_value(stacky, &operand_b);
  if(success_b != STACKY_OK) {
    return success_b;
  }

  StackyErrorState success_a = stacky_pop_value(stacky, &operand_a);
  if(success_a != STACKY_OK) {
    return success_a;
  }

  int64_t result = operand_a * operand_b;

  return stacky_push_value(stacky, result);
}

/*
 * Description: Execute the next instruction that the instruction_pointer looks at.
 * @stacky - The virtual machine.
 */
StackyErrorState stacky_execute_cycle(Stacky *stacky) {
  //TODO(Kay): The instruction_pointer++ might bite us in the ass later when there are relative jumps from the current
  //           position. We should keep that in mind!
  StackyInstruction instruction = stacky->code_segment[stacky->instruction_pointer++]; 
  StackyErrorState result = STACKY_OK;

  switch(instruction.opcode) {
    case INSTRUCTION_PUSH: {
      result = stacky_push_value(stacky, instruction.argument);
      break;
    } 
    case INSTRUCTION_POP: {
      int64_t value = 0;
      result = stacky_pop_value(stacky, &value);
      break;
    }
    case INSTRUCTION_ADD: {
      assert(instruction.argument == 0);
      result = stacky_instruction_add(stacky);
      break;
    }
    case INSTRUCTION_SUB: {
      assert(instruction.argument == 0);
      result = stacky_instruction_sub(stacky);
      break;
    }
    case INSTRUCTION_DIV: {
      assert(instruction.argument == 0);
      result = stacky_instruction_div(stacky);
      break;
    }
    case INSTRUCTION_MUL: {
      assert(instruction.argument == 0);
      result = stacky_instruction_mul(stacky);
      break;
    }
    case INSTRUCTION_DUMP: {
      result = stacky_dump_value(stacky);
      break;
    }
    case INSTRUCTION_HALT: {
      stacky->halted = true;
      break;
    }
  }

  return result;
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

/*
 * Description: Read Stacky Byte Code from file into the code_segment of the virtual machine.
 * @stacky: virtual machine where the code should be loaded into.
 * @file_path: file to read the instructions from.
 */
void stacky_bce_read_byte_code_from_disk(Stacky *stacky, const char *file_path) {
  assert(file_path != NULL);

  FILE *input_file_handle = fopen(file_path, "rb");
  if(input_file_handle == NULL) {
    fprintf(stderr, "stacky_byte_code_engine error: cannot open file %s: %s\n", file_path, strerror(errno));
    exit(1);
  }

  if(fseek(input_file_handle, 0, SEEK_END) < 0) {
    fprintf(stderr, "stacky_byte_code_engine fseek error %s: %s\n", file_path, strerror(errno));
    fclose(input_file_handle);
    exit(1);
  }
  
  size_t file_size = ftell(input_file_handle);
  rewind(input_file_handle);
  size_t instruction_count = file_size / sizeof(StackyInstruction);
  if(instruction_count >= CODE_SEGMENT_MAX_CAPACITY) {
    fprintf(stderr, "stacky_byte_code_engine error %s: contains too many instructions\n", file_path);
    fclose(input_file_handle);
    exit(1);
  }

  
  stacky->code_segment_size = fread(stacky->code_segment, sizeof(StackyInstruction), instruction_count, input_file_handle);
  fclose(input_file_handle);
}

/*
 * Description: Write the code_segment into the specified file as byte code.
 * @instructions: The buffer containing the instructions
 * @code_segment_size: count of instructions inside the buffer
 * @file_path: file to write the generated byte code into.
 */
void stacky_bce_write_byte_code_to_disk(StackyInstruction *instructions, size_t code_segment_size, const char *file_path) {
  assert(file_path != NULL);
  assert(instructions != NULL);
  assert(code_segment_size > 0);

  FILE *output_file_handle = fopen(file_path, "wb");
  if(output_file_handle == NULL) {
    fprintf(stderr, "stacky_byte_code_engine error: cannot open file %s: %s\n", file_path, strerror(errno));
    exit(1);
  }

  size_t bytes_written = fwrite(instructions, sizeof(StackyInstruction), code_segment_size, output_file_handle);
  if(bytes_written < code_segment_size) {
    fprintf(stderr, "stacky_byte_code_engine error: cannot write bytes to file %s: %s\n", file_path, strerror(errno));
    fclose(output_file_handle);
    exit(1);
  }

  fclose(output_file_handle);
}

char *stacky_assembler_load_source_code_from_file(const char *file_path) {
  assert(file_path != NULL);

  FILE *source_file_handle = fopen(file_path, "r");
  if(source_file_handle == NULL) {
    fprintf(stderr, "stacky_assembler error: cannot open file %s: %s\n", file_path, strerror(errno));
    exit(1);
  }

  if(fseek(source_file_handle, 0, SEEK_END) < 0) {
    fprintf(stderr, "stacky_byte_code_engine fseek error %s: %s\n", file_path, strerror(errno));
    fclose(source_file_handle);
    exit(1);
  }
  
  size_t file_size = ftell(source_file_handle);
  rewind(source_file_handle);

  char *buffer = calloc(file_size, sizeof(char));
  if(buffer == NULL) {
    fprintf(stderr, "stacky_assembler error cannot allocate memory for buffer: %s\n", strerror(errno));
    fclose(source_file_handle);
    exit(1);
  }
  
  size_t bytes_read = fread(buffer, sizeof(char), file_size, source_file_handle);
  assert(bytes_read <= file_size);

  fclose(source_file_handle);

  return buffer;
}

StackyInstruction stacky_assemble_instruction(LString source_line) {
  assert(source_line.data != NULL);

  LString instruction = lstring_split_by_delimiter(&source_line, ' ');
  StackyInstruction generated_instruction;

  if(strncmp(instruction.data, "push", 4) == 0) {
    if(source_line.length == 0) {
      fprintf(stderr, "stacky_compiler error: mnemonic `push` is missing a value that is pushed onto the stack original instruction is `%.*s`\n",
              (int)source_line.length, source_line.data);
    }
    LString value = lstring_split_by_delimiter(&source_line, ' ');
    int64_t converted_value = lstring_to_integer_value(value);
    generated_instruction = (StackyInstruction) { INSTRUCTION_PUSH, converted_value };
  }
  else if (strncmp(instruction.data, "add", 3) == 0) {
    generated_instruction = (StackyInstruction) { INSTRUCTION_ADD, 0 };
  }
  else if (strncmp(instruction.data, "dump", 4) == 0) {
    generated_instruction = (StackyInstruction) { INSTRUCTION_DUMP, 0 };
  }
  else {
    fprintf(stderr, "stacky_compiler error: unknown mnemonic with a length of %ld and a representation of `%.*s` encountered!\n", instruction.length, (int)instruction.length, instruction.data);
  }

  return generated_instruction;
}
void stacky_assemble_file(Stacky *stacky, LString source_code) {
  assert(source_code.data != NULL);

  while(source_code.length > 0) {
    LString source_line = lstring_split_by_delimiter(&source_code, '\n');
    StackyInstruction assembled_instruction = stacky_assemble_instruction(source_line);
    stacky_push_code_instruction(stacky, assembled_instruction);
  }
}

int main(void) {
  #ifdef TEST_MODE
    test_main();
  #else
  //TODO(Kay): Add Make_Instruction Function
  Stacky stacky_vm = {0};
  const char *source_code_cstring = stacky_assembler_load_source_code_from_file("examples/addition.asm");
  LString source_code = lstring_from_cstring(source_code_cstring);

  stacky_assemble_file(&stacky_vm, source_code);

  //stacky_bce_read_byte_code_from_disk(&stacky_vm, "test_output.sbc");

  for(size_t i = 0; i < stacky_vm.code_segment_size && !stacky_vm.halted; i++) {
    StackyErrorState vm_error_state = stacky_execute_cycle(&stacky_vm);

    if(vm_error_state != STACKY_OK) {
      exit(vm_error_state);
    }
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
