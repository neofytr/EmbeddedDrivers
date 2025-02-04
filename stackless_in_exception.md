### Why `__stackless` and `naked` are Required for Exception Handlers

Exception handlers, especially those used for low-level system resets and fault recovery, often need to be marked as `naked` (or equivalent in other compilers) and avoid using a stack (`__stackless` concept). The reasons for this are primarily related to:

1. **Stack Integrity During Exceptions**  
2. **Minimal Overhead and Predictability**  
3. **Avoiding Compiler-Generated Prologue/Epilogue Code**  

We will analyze each in detail, then examine what happens if we *do* use a stack in such handlers.

---

## **1. Stack Integrity During Exceptions**
When an exception occurs (e.g., a fault or system reset request), the CPU typically pushes several registers onto the stack. However, under certain conditions, the stack may be in an **unusable or corrupted state**, such as:

- **Stack Pointer Corruption:** If the stack pointer (SP) was overwritten due to a memory error, buffer overflow, or invalid pointer dereference.
- **Stack Overflow:** If an interrupt or fault occurs when the stack is already exhausted.
- **Stack Inaccessibility:** If the stack is in an uninitialized or protected memory region (e.g., if an MPU region makes it inaccessible).

If the exception handler tries to use the stack, it could cause a secondary fault (often a *hard fault*) that escalates into an unrecoverable error. One case could be that the hard fault handler itself pushes onto stack, again causing the hard fault exception, invoking the handler that repeats the same thing in an infinite loop (system freezing).

By marking the function as `naked`, the compiler **does not generate function prologue/epilogue**, preventing it from:
- Pushing registers onto an invalid stack.
- Adjusting an already corrupt SP.
- Using local variables that require stack allocation.

This ensures that the exception handler executes without relying on a potentially broken stack.

---

## **2. Minimal Overhead and Predictability**
Exception handlers must execute quickly and predictably. If the compiler generates unnecessary stack operations:

- **Latency Increases:** Extra instructions slow down the exception response time.
- **Execution Becomes Less Deterministic:** The behavior depends on compiler optimizations and stack state.
- **Context Switching Becomes Unreliable:** If an exception handler itself modifies the stack state, it could affect nested exceptions.

By using `naked`, we ensure that:
1. The handler is purely assembly-level control without extra stack interactions.
2. The CPU immediately jumps to the exception routine without unnecessary instructions.

---

## **3. Avoiding Compiler-Generated Prologue/Epilogue Code**
Normally, when a function is called in C, the compiler generates:
- A function **prologue**: Saves registers, allocates space on the stack.
- A function **epilogue**: Restores registers, deallocates the stack, and returns.

For example, a typical function might generate:

```assembly
push {r4, lr}   // Prologue: Save registers
...
pop {r4, pc}    // Epilogue: Restore registers and return
```

However, in an **exception handler**, this is problematic because:
1. If the stack is invalid, `push` will fail or corrupt memory.
2. If the function returns using `pop {pc}`, it may return to an invalid or unintended location.
3. Exception handlers typically do not return normally; they either:
   - **Loop indefinitely** (`for(;;)`)
   - **Trigger a system reset**
   - **Return using special exception return sequences** (`BX LR` with EXC_RETURN values)

The `naked` attribute prevents the compiler from generating these prologue/epilogue sequences, allowing **manual control** of what gets executed.

---

## **What Happens If We Use a Stack?**
If an exception handler **uses the stack**, the following issues can arise:

### **1. Corrupting the Fault Handling Path**
If an exception occurs due to a stack overflow or corruption, and the handler attempts to push to the stack, it could:

- **Overwrite valid memory regions**
- **Cause a fault within the fault handler**, leading to a system lockup

This is a common cause of "double fault" conditions.

### **2. Stack Pointer Might Not Be Valid**
When an exception occurs, the stack pointer (`SP`) may be:
- Pointing to an invalid address.
- Misdirected due to an incorrect memory write.
- Already exhausted, meaning further pushes cause memory corruption.

Using the stack blindly could result in immediate failure.

### **3. Nested Exception Issues**
If an exception handler pushes registers to an already corrupted stack, then another exception occurs, the **stack state will be unrecoverable**, possibly leading to a **triple fault** (on Cortex-M, this leads to a CPU reset).

### **4. Extra Overhead During Critical Faults**
For a function meant to reset the system (like `system_reset`), pushing registers and modifying the stack serves **no purpose** and only adds unnecessary overhead.

---

## **Conclusion**
The `naked` and `__stackless` approach ensures:
- The exception handler executes reliably even when the stack is invalid.
- No unnecessary instructions are generated that could worsen a fault.
- The handler remains minimal and predictable, ensuring quick execution.

Thus, `naked` and `__stackless` exception handlers are crucial for system reliability, especially in critical fault-handling scenarios.