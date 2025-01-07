ARM Cortex-M processors use a range of **branch instructions** to manage program control flow, function calls, and conditional execution. 

---

### **Branch Instructions in ARM Cortex-M**
1. **B (Branch)**
   - The `B` instruction performs an **unconditional branch** to a target address.
   - Syntax: `B label`
   - It modifies the Program Counter (PC) to point to the specified label or address.
   - Example:
     ```assembly
     B loop
     ```
     This will jump to the label `loop`.

2. **BL (Branch with Link)**
   - The `BL` instruction is used for **function calls**. It branches to the target address and stores the return address (PC + 4) in the Link Register (LR).
   - Syntax: `BL label`
   - Example:
     ```assembly
     BL function
     ```

3. **BX (Branch and Exchange)**
   - The `BX` instruction branches to an address stored in a register and determines the processor's **instruction set state** (Thumb or ARM) using the least significant bit (LSB) of the target address:
     - LSB = `1`: Thumb state (used by Cortex-M processors).
     - LSB = `0`: ARM state (not used in Cortex-M; only Thumb is supported).
   - Syntax: `BX Rn`
   - Example:
     ```assembly
     BX LR
     ```

4. **BLX (Branch with Link and Exchange)**
   - Similar to `BL`, but it can branch to an address in a register (like `BX`) while saving the return address in LR.
   - It can also switch between Thumb and ARM states based on the LSB of the target address.
   - Syntax: `BLX Rn`

5. **CBZ/CBNZ (Compare and Branch on Zero/Non-Zero)**
   - These instructions perform a conditional branch based on the value of a register:
     - `CBZ`: Branch if the register value is zero.
     - `CBNZ`: Branch if the register value is non-zero.
   - Syntax:
     ```assembly
     CBZ Rn, label
     CBNZ Rn, label
     ```
   - Example:
     ```assembly
     CBZ R0, done ; If R0 is zero, branch to "done"
     ```

6. **Conditional Branch Instructions**
   - Conditional branches are executed based on the state of condition flags (N, Z, C, V) in the Program Status Register (PSR).
   - Examples include `BEQ`, `BNE`, `BGT`, `BLT`, etc.
   - Syntax: `B<cond> label`
     - `BEQ`: Branch if equal (Z=1).
     - `BNE`: Branch if not equal (Z=0).
     - `BGT`: Branch if greater than (Z=0, N=V).
     - `BLT`: Branch if less than (N≠V).
   - Example:
     ```assembly
     CMP R0, #5
     BEQ match ; Branch if R0 == 5
     ```

---

### **Function Call and Return Mechanism**

1. **Function Call**
   - ARM Cortex-M uses the **BL** instruction for function calls:
     - Saves the return address in the **Link Register (LR)**.
     - Updates the **Program Counter (PC)** to the target address.

   - Example:
     ```assembly
     BL my_function ; Calls "my_function"
     ```
     Inside the function:
     - The first instruction is executed at the address `my_function`.
     - The return address is stored in `LR`.

2. **Function Return**
   - The **BX LR** instruction is used to return from a function:
     - Copies the return address from LR back into the PC.
   - Example:
     ```assembly
     BX LR ; Return to the caller
     ```

   - If nested functions are called, the **stack** is used to save/restore LR:
     - Push LR onto the stack before calling another function.
     - Pop LR from the stack before returning.

3. **Stack Usage**
   - When a function is called:
     - The return address (LR) and other necessary registers are pushed onto the stack.
   - When returning:
     - The stack is restored to its previous state.

---

### **The Last Bit in the Target Address**

1. **Thumb State Enforcement**
   - ARM Cortex-M processors exclusively execute **Thumb instructions**, which are 16-bit or 32-bit in size. To enforce this, the LSB of the branch target address must always be `1`.
   - For example:
     - `0x08000000` (LSB = 0): ARM state (not valid for Cortex-M).
     - `0x08000001` (LSB = 1): Thumb state (valid for Cortex-M).

2. **How It Works**
   - The hardware uses the LSB of the address to determine the instruction set state:
     - LSB = 1: Thumb state.
     - LSB = 0: ARM state (not supported in Cortex-M).
   - The processor automatically ignores the LSB when setting the PC.

3. **Branch and Exchange (BX) and LSB**
   - The `BX` instruction checks the LSB of the target register:
     - If the LSB is `1`, it continues in Thumb state.
     - If the LSB is `0`, the behavior is unpredictable in Cortex-M processors.

---

### **Detailed Example: Function Call and Return**

```assembly
; Main program
    LDR R0, =0x20000000 ; Load an address into R0
    BL my_function       ; Call "my_function"
    B main_loop          ; Infinite loop

my_function:
    PUSH {LR}            ; Save LR on the stack
    ADD R0, R0, #1       ; Increment R0
    POP {LR}             ; Restore LR
    BX LR                ; Return to caller

main_loop:
    B main_loop          ; Infinite loop
```

- **Explanation:**
  - The `BL` instruction saves the return address in `LR`.
  - Inside `my_function`, `LR` is pushed to the stack to preserve it.
  - After performing operations, `LR` is restored, and `BX LR` returns control to the calling function.

---

### **Key Points to Remember**
- The LSB of the target address for branch instructions determines the instruction set state (Thumb for Cortex-M).
- Function calls use `BL` to save the return address in `LR` and `BX LR` to return.
- Stack management is critical for nested function calls to preserve the return address and other register values.
- Conditional branches rely on the flags in the PSR for decision-making.