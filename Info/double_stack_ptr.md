### **Main Stack Pointer (MSP) and Process Stack Pointer (PSP) in ARM Cortex-M**
The **Main Stack Pointer (MSP)** and **Process Stack Pointer (PSP)** are two separate stack pointers in ARM Cortex-M processors, designed to support different execution contexts, particularly for privileged vs. unprivileged execution modes. 

---

## **1. Stack Organization in ARM Cortex-M**
### **Memory Regions for Stacks**
Both MSP and PSP reside in **SRAM (System RAM)**, but they are typically placed in different regions. The stack regions are defined by the linker script or the system memory layout.

- **MSP (Main Stack) typically starts at the highest address of SRAM** and grows downwards.
- **PSP (Process Stack) is allocated separately and grows downwards as well.** It is usually assigned in an RTOS or a multi-tasking environment for user tasks.

#### **Example of Typical Stack Allocation in SRAM**
Assume an MCU with **64 KB of SRAM (0x2000_0000 - 0x2000_FFFF)**:

| Address Range  | Description |
|---------------|------------|
| **0x2000_FFFF** | Initial MSP (Highest SRAM Address) |
| ...           | MSP Stack Growing Downward |
| **0x2000_C000** | Possible PSP Base |
| ...           | PSP Stack Growing Downward |
| **0x2000_8000** | Heap or Global Variables |
| **0x2000_0000** | Start of SRAM |

- The **MSP is used by default after reset** and serves as the system stack (for exceptions, privileged mode).
- The **PSP is used for user tasks**, which are typically scheduled by an RTOS.

---

## **2. Control Register and Stack Selection**
The **CONTROL** register determines whether MSP or PSP is in use.

| CONTROL Register Bit | Meaning |
|---------------------|---------|
| **CONTROL[0]** = 0 | Privileged mode |
| **CONTROL[0]** = 1 | Unprivileged mode |
| **CONTROL[1]** = 0 | Use **MSP** |
| **CONTROL[1]** = 1 | Use **PSP** |

To select PSP:
```c
__set_CONTROL(__get_CONTROL() | (1 << 1)); // Set CONTROL[1] to use PSP
```
To check the active stack pointer:
```c
if (__get_CONTROL() & (1 << 1)) {
    // PSP is active
} else {
    // MSP is active
}
```

---

## **3. Can MSP and PSP Overlap?**
### **Possible Overlap**
Both MSP and PSP reside in the **same SRAM**, and if not properly managed, they can overlap. However, overlap **should be avoided**, as stack corruption can occur when one stack overflows into the other.

- **In a well-defined system**: The memory layout ensures that MSP and PSP are allocated separate regions within SRAM.
- **In an RTOS**: The PSP is typically assigned a separate stack for each thread, managed by the OS.

### **Stack Overflow Risks**
- If an interrupt occurs while using PSP and MSP is too small, the **MSP stack could overflow into the PSP stack**.
- If user tasks using PSP exceed their allocated memory, they can corrupt data structures in MSP.

---

## **4. MSP and PSP Usage in Different Execution Contexts**
| Context          | Stack Pointer Used |
|-----------------|------------------|
| **After Reset** | MSP (Default) |
| **Exception Handling (Interrupts, Faults)** | MSP |
| **Handler Mode (Privileged)** | MSP |
| **Thread Mode (Privileged)** | MSP (default) or PSP (if set) |
| **Thread Mode (Unprivileged)** | PSP |

### **How an RTOS Uses PSP**
- The **kernel (privileged mode)** runs on **MSP**.
- Each **user task (unprivileged mode)** runs on its **own PSP stack**.
- During **context switching**, the RTOS changes the PSP to switch between tasks.

---

## **5. Example: Switching from MSP to PSP**
This example shows how an RTOS might configure a task to use PSP:
```c
uint32_t user_stack[256];  // Allocate stack space for a user task

// Set PSP to point to the end of user stack
__set_PSP((uint32_t)(user_stack + 256));

// Switch to use PSP in thread mode
__set_CONTROL(__get_CONTROL() | (1 << 1));

// Ensure execution follows the change
__ISB();
```

---

## **6. Summary**
| Feature  | MSP (Main Stack Pointer) | PSP (Process Stack Pointer) |
|----------|------------------|------------------|
| **Default After Reset** | ✅ Yes | ❌ No |
| **Used in Interrupts** | ✅ Yes | ❌ No |
| **Used in Privileged Mode** | ✅ Yes | ✅ Possible |
| **Used in Unprivileged Mode** | ❌ No | ✅ Yes |
| **Stack Location** | Typically at top of SRAM | Assigned separately |
| **Risk of Overlap** | Possible if mismanaged | Possible if mismanaged |

---
### **Final Notes**
- **MSP and PSP reside in SRAM** but are managed separately to avoid overlap.
- **MSP is always used for exceptions and system-level operations.**
- **PSP is used for user-level execution** in RTOS/multi-tasking scenarios.
- **Proper memory allocation** prevents stack corruption between MSP and PSP.
