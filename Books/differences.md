### **1. Cortex-M4 Processor Technical Reference Manual (TRM)**  
- **Purpose:**  
  - Provides detailed documentation on the **Cortex-M4 processor core** itself, including its architecture, registers, exception handling, memory system, and debug features.  
- **Scope:**  
  - Focuses specifically on the **Cortex-M4 core** (excluding any vendor-specific implementations).  
  - Covers features like **NVIC (Nested Vectored Interrupt Controller), SysTick timer, MPU (Memory Protection Unit), FPU (Floating-Point Unit)**, etc.  
- **Use Case:**  
  - If you are writing **low-level software** (e.g., OS/kernel development) or need to understand the internals of the **Cortex-M4 core** in detail.  

**📄 Example Document:**  
[Cortex-M4 TRM (DUI0553)](https://developer.arm.com/documentation/dui0553/latest/)

---

### **2. ARMv7-M Instruction Set Architecture (ISA) Manual**  
- **Purpose:**  
  - Defines the **instruction set** used by ARM Cortex-M processors, including instruction formats, encodings, and behavior.  
- **Scope:**  
  - Covers all instructions available in the **ARMv7-M architecture**, which is implemented by **Cortex-M3 and Cortex-M4**.  
  - Includes both **Thumb and Thumb-2** instructions.  
  - Covers conditional execution, data processing, branching, load/store, and system-level instructions.  
- **Use Case:**  
  - If you are writing **assembly code**, developing **compilers**, or need a detailed understanding of **CPU instructions**.  

**📄 Example Document:**  
[ARMv7-M Architecture Reference Manual (DDI0403)](https://developer.arm.com/documentation/ddi0403/latest/)  

---

### **3. Cortex-M4 Devices Generic User Guide**  
- **Purpose:**  
  - A high-level guide on how to use Cortex-M4-based **microcontrollers** (like STM32, NXP, etc.), focusing on **how to program and interact with peripherals** rather than the CPU itself.  
- **Scope:**  
  - Explains things like **system initialization, clock setup, exception handling, and power management**.  
  - Covers **CMSIS (Cortex Microcontroller Software Interface Standard)**, which helps with **software portability** across different Cortex-M devices.  
- **Use Case:**  
  - If you are working on **firmware development** and need a broad understanding of **how to program a Cortex-M4-based microcontroller**.  

**📄 Example Document:**  
[Cortex-M4 Generic User Guide (DUI0552)](https://developer.arm.com/documentation/dui0552/latest/)

---

### **Summary of Differences**  
| Manual | Focus | Key Details | Best For |
|--------|--------|------------|----------|
| **Cortex-M4 TRM** | Core architecture | Covers registers, memory, NVIC, MPU, FPU, debugging | Kernel/OS development, hardware design |
| **ARMv7-M ISA Manual** | Instruction set | Describes Thumb and Thumb-2 instructions, encoding, and execution | Assembly programming, compiler development |
| **Cortex-M4 Generic User Guide** | Microcontroller usage | Describes system setup, CMSIS, exception handling | Embedded software, firmware development |
