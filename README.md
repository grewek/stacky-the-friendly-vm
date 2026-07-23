# Stacky the friendly vm

Stacky is a implementation of a stack based virtual machine. The project should be finished in a
40 hour time frame.

## Current State

The virtual machine currently only supports 64-bit integers. Support for strings and boolean values is planned.
The virtual machine currently has support for a few basic opcodes like pushing values or doing math operations


## Build

To build the project clone it to your local machine and run the makefile:

```console
$ git clone https://grewek/stacky-the-friendly-vm.git
$ make stacky
$ ./stacky
```

## Usage

### Compile

To compile assembly into usable bytecode you can use the compile CLI flag:

`$ ./stacky compile path/to/file output_file.sbc` where `path/to/file` is the actual path to the assembly that needs to be compiled. The name and path
of the second parameter can be freely chosen.

### Run

To run bytecode that was compiled for stacky use the `run` CLI command:

`$ ./stacky run path/to/bytecode_file` again `path/to/bytecode_file` is the bytecode file to execute.

## Data Segment

Stacky can store integers, strings and arrays of integers, these last two elements however are not stored directly 
onto the stack, instead they are stored inside the data segment of the `.sbc` file. The stack only contains a pointer
to the actual data, also arrays have a maximum size of 1024 elements per array instance. This is due to time constraints 
as a variable sized array would need the implementation of a dynamic array.

## Instructions

Currently these Instructions are supported by the virtual machine

### Push

Pushes the value to the stack.

Possible arguments: __Immediate value__, __String__, __Array__

Syntax: `push [immediate value] | push "<string>" | push [<number> <number> ...]`

__Note__: Values inside of an array are seperated by a space not a comma.

### Pop

Removes the current value at the top of the stack

Arguments: __None__

Syntax: `pop`

### Duplicate

Duplicate the value on the top of the stack, useful for operations that consume the value

__Argument__: __None__

Syntax: `dup`

### Add

Adds the two values at the top of the stack.

Argument: __None__

Syntax: `add`

### Sub

Subtracts the two values on the top of the stack and push the result.

Argument: __None__

Syntax: `sub`

### Mul

Multiply the two values at the top of the stack and push the result.

Argument: __None__

Syntax: `mul`

### Div

Divide the two values at the top of the stack and push the result.

Argument: __None__

Syntax: `div`

### Compare

Compares the two top values by doing a subtraction, then stores the result onto the top of the stack

Argument: __None__

Syntax: `cmp`

### Jump

Jump to the absolute address and resume execution from there.

Argument: __Absolute Address__ (Boundaries: Addr >= 0)

Syntax: `jmp [address]`

### Jump Equal

Jump to the absolute address when the compared value on the top of the stack __is zero__

Argument: __Absolute Address__ (Boundaries: Addr >= 0)

Syntax: `je [address]`

### Jump Not Equal

Jump to the absolute address when the compared value on the top of the stack __is not zero__

Argument: __Absolute Address__ (Boundaries: Addr >= 0)

Sytnax: `jne [address]`

### Dump

Print the value that is on top of the stack to the screen.

Argument: __None__

Syntax: `dump`

### Halt

Put the vm in a halted state

Argument: __None__

Syntax: `halt`
## Container

(Section needs to be written)
