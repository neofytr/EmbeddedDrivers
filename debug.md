# Debugging steps

1. st-util
2. arm-none-eabi-gdb output.bin
3. target extended-remote:4242
4. monitor reset halt 
5. load

reset:

1. Issues a hardware reset to the microcontroller
2. This clears the CPU registers, resets the program counter to the reset vector, and restarts the system's initialization process.

halt:

1. Halt the CPU immediately after the reset, before the application starts running.
2. This allows the debugger to take control before the program executes any instructions.

load:

Transfers the elf file from the host machine to the microcontrollers memory


# Some trivial info

PC is altered after the instruction has finished executing, by the size of the current instruction to go to the next instruction.

So, when a branch instruction is executing, the PC contains the location of that branch instruction.

So, the linker replaces the label by the offset of the label relative to the branch instruction itself.