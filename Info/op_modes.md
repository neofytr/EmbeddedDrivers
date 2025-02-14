## **Execution Modes and Privilege Levels in Cortex-M4: A Formal and Detailed Explanation**  

The ARM Cortex-M4 processor has two **execution modes** and two **privilege levels**, which work together to control the behavior of code execution, system access, and security.

---

# **1. Execution Modes (Operational Modes)**  

The execution mode determines **how the processor operates and when control is transferred between different execution contexts**.  

### **1.1 Thread Mode**
- The default execution mode **after reset (boot-up).**. The privilege level is **Privileged** after **reset**.
- Used for executing **normal application code** (either as part of an operating system or a bare-metal application).
- Can run in **Privileged** or **Unprivileged** mode.
- If an OS is present, it will typically run the kernel in **Privileged Thread Mode** and applications in **Unprivileged Thread Mode**.

### **1.2 Handler Mode**
- Entered when the processor handles **exceptions or interrupts**.
- Used to run **Interrupt Service Routines (ISRs)** and **exception handlers**.
- Always runs in **Privileged Mode**, ensuring that **interrupts and exception handlers** have full system access.

---

# **2. Privilege Levels**  

The **privilege level** determines the **degree of access a program has to system resources** such as registers, system memory, and control functions.

### **2.1 Privileged Mode**
- Has full access to:
  - **System Control Registers** (e.g., System Control Block, NVIC, MPU).
  - **Memory Protection Unit (MPU)** configurations.
  - **Interrupt configuration and priority registers**.
- Required for **modifying system configurations** or enabling/disabling interrupts.
- Executed in **Handler Mode** (always privileged) and optionally in **Thread Mode**.

### **2.2 Unprivileged Mode**
- Restricted access:
  - **Cannot modify system control registers**.
  - **Cannot disable or enable interrupts**.
  - **Cannot modify MPU settings** (if enabled).
  - **Cannot access certain memory regions** if an MPU is configured.
- Typically used for **user applications** in an OS.
- Only available in **Thread Mode**.

---

# **3. Why Do We Need Both Execution Modes and Privilege Levels?**
### **3.1 Why Two Privilege Levels?**
- **Security & Isolation:** Prevents user code from corrupting critical system configurations.
- **Fault Containment:** A bug in unprivileged code will not crash the entire system.
- **Operating System Support:** Enables **kernel-level privilege separation** between system processes and user applications.

### **3.2 Why Two Execution Modes?**
- **Thread Mode** allows **normal execution** of application or OS kernel code.
- **Handler Mode** ensures that **interrupts and exceptions** always have privileged access.

**If execution modes did not exist:**  
- Interrupts might inherit the privilege level of the running thread, potentially preventing critical system actions.  
- The OS could not enforce strict separation between application and kernel code.

---

# **4. Switching Between Execution Modes and Privilege Levels**
## **4.1 Switching Between Privilege Levels**
### **4.1.1 Switching from Privileged to Unprivileged Mode**
- The transition can only be done **inside Thread Mode** by modifying the `CONTROL` register.
- `CONTROL[0]` (bit 0) controls the privilege level:
  - `0` → Privileged
  - `1` → Unprivileged
- Example in C:
  ```c
  __asm volatile (
      "mov r0, #1 \n"  // Set bit 0 of CONTROL register to 1 (Unprivileged)
      "msr CONTROL, r0 \n"  // Write to CONTROL register
      "isb \n"  // Instruction Synchronization Barrier (flush pipeline)
  );
  ```
- **Once switched to Unprivileged mode, the processor CANNOT switch back to Privileged mode directly**.

### **4.1.2 Switching from Unprivileged to Privileged Mode**
- Unprivileged code **cannot modify CONTROL directly**.
- The only way to regain **Privileged Mode** is via an **exception or system call (SVC instruction)**.

- Example: Trigger a Supervisor Call (SVC) to regain privileges:
  ```c
  __asm volatile ("svc #0");
  ```
- The SVC handler (running in **Handler Mode, Privileged**) modifies the `CONTROL` register to restore **Privileged Mode**.

---

## **4.2 Switching Between Execution Modes**
### **4.2.1 Switching from Thread Mode to Handler Mode**
- Occurs **automatically** when:
  - An **interrupt** is triggered.
  - An **exception** occurs (e.g., Faults, SVC calls).
- The processor:
  1. Saves the current execution context on the stack.
  2. Switches to **Handler Mode (Privileged)**.
  3. Jumps to the **vector table** to execute the ISR or exception handler.

### **4.2.2 Switching from Handler Mode to Thread Mode**
- Happens **automatically when an ISR returns** using the `BX LR` instruction.
- The processor:
  1. Restores the **saved execution context** from the stack.
  2. Resumes execution in **Thread Mode** at the point where the interrupt occurred.
- If the interrupted code was **Unprivileged**, the processor will return to **Unprivileged Mode**.

---

## **5. Complete Transition Table**
| Current State | Action | New State |
|--------------|--------|----------|
| **Thread Mode (Privileged)** | Modify `CONTROL` register | **Thread Mode (Unprivileged)** |
| **Thread Mode (Unprivileged)** | Issue `SVC` exception | **Handler Mode (Privileged)** |
| **Thread Mode** | Trigger an **interrupt** | **Handler Mode (Privileged)** |
| **Handler Mode** | Return from exception | **Thread Mode (previous privilege level restored)** |

---

## **6. Example Code: Demonstrating Mode and Privilege Switching**
```c
#include <stdint.h>

void SVC_Handler(void) {
    __asm volatile (
        "mov r0, #0 \n" // Set CONTROL register bit[0] to 0 (Privileged Mode)
        "msr CONTROL, r0 \n"
        "isb \n"
    );
}

void switch_to_unprivileged() {
    __asm volatile (
        "mov r0, #1 \n" // Set CONTROL register bit[0] to 1 (Unprivileged Mode)
        "msr CONTROL, r0 \n"
        "isb \n"
    );
}

int main() {
    // Initially in Thread Mode (Privileged)
    switch_to_unprivileged();  // Moves to Unprivileged Thread Mode

    // Now in Unprivileged Mode
    __asm volatile ("svc #0");  // Call SVC, triggers transition to Handler Mode (Privileged)

    // Execution resumes here in Privileged Thread Mode
    while (1);
}
```

---

## **7. Conclusion**
- **Execution modes (Thread & Handler) define the processor’s behavior during execution and exceptions.**
- **Privilege levels (Privileged & Unprivileged) define access control for system resources.**
- **Modes and privilege levels work together to enforce system security, stability, and OS functionality.**
- **Transitions between these states occur via system calls, interrupts, and exception handling mechanisms.**

