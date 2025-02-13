# **Using SWO for `printf` Debugging on STM32 (Step-by-Step Guide)**  

---

## **🔍 What is SWO (Serial Wire Output)?**  
SWO (**Serial Wire Output**) is part of the **Serial Wire Debug (SWD) interface** in ARM Cortex-M microcontrollers. It allows you to send **printf-style debugging messages** from your STM32 to your debugger **without using UART**.  

SWO is a **one-wire** channel for real-time data output. It is often used for:  
- **Logging and debugging without halting execution**  
- **Profiling CPU events and performance monitoring**  
- **Tracing variable values in real-time**  

### **📌 Why Use SWO Instead of UART for Debugging?**  
| Feature | SWO | UART |
|---------|-----|------|
| **Number of Wires** | 1 | 2 (TX, RX) |
| **Speed** | High-speed | Lower compared to SWO |
| **Interrupt Usage** | None (no RX needed) | Requires interrupts or DMA for RX |
| **Debugging While Running?** | ✅ Yes | ❌ Usually requires halting |

---

## **🔧 Step 1: Understanding SWO Hardware and Pin Mapping**  
### **📌 Where is the SWO Pin?**
- SWO is typically mapped to **PB3** (STM32F401, F4, and other STM32 devices).  
- The **ST-LINK debugger** captures SWO data and sends it to the PC over USB.  

| STM32 Pin | Function |
|-----------|----------|
| **PB3** | **SWO (Serial Wire Output)** |
| **PA13** | **SWDIO (Serial Wire Debug I/O)** |
| **PA14** | **SWCLK (Serial Wire Clock)** |

---

## **🛠️ Step 2: Configuring SWO on STM32 (Bare Metal)**  
To enable SWO output, you need to configure:  
1. **The SWO pin (PB3) as an alternate function (AF0)**.  
2. **The Debug peripheral (`DBGMCU`) to enable SWO.**  
3. **The Trace Port Interface Unit (TPIU) to format the output.**  
4. **The Instrumentation Trace Macrocell (ITM) to send data.**  

### **📝 Code for Bare Metal SWO Setup (STM32F401)**
```c
#include "stm32f4xx.h"

void SWO_Init(uint32_t baudrate) {
    // Enable GPIOB clock (SWO is on PB3)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // Configure PB3 as Alternate Function (AF0 for SWO)
    GPIOB->MODER   &= ~(3U << (3 * 2));  // Clear mode bits
    GPIOB->MODER   |=  (2U << (3 * 2));  // Set Alternate Function mode
    GPIOB->OSPEEDR |=  (3U << (3 * 2));  // Set high speed
    GPIOB->AFR[0]  |=  (0U << (3 * 4));  // Set AF0 for PB3

    // Enable debugging in low-power modes
    DBGMCU->CR |= DBGMCU_CR_TRACE_IOEN;

    // Configure Trace Port Interface Unit (TPIU)
    TPI->ACPR = (SystemCoreClock / baudrate) - 1; // Set SWO baudrate
    TPI->SPPR = 2;  // Select Manchester encoding (if needed)

    // Enable ITM stimulus port 0 (for printf)
    ITM->LAR = 0xC5ACCE55;  // Unlock ITM
    ITM->TCR = (1U << 23) |  // Enable ITM
               (1U << 3)  |  // Enable SWO output
               (1U << 0);    // Enable ITM globally
    ITM->TER |= 1;           // Enable ITM stimulus port 0
}

void SWO_PrintChar(char ch) {
    if (ITM->TCR & ITM_TCR_ITMENA) {  // Ensure ITM is enabled
        while (ITM->STIM[0] == 0);  // Wait for buffer to be ready
        ITM->STIM[0] = ch;          // Send character
    }
}

void SWO_PrintString(const char *str) {
    while (*str) {
        SWO_PrintChar(*str++);
    }
}
```

### **📌 Explanation of the Code**
- **Enables the GPIO clock for PB3** (used for SWO output).
- **Configures PB3 as Alternate Function (AF0)**.
- **Sets the SWO baud rate** based on the system clock.
- **Enables the ITM (Instrumentation Trace Macrocell)**.
- **Uses `ITM->STIM[0]` to send characters over SWO**.

---

## **🛠️ Step 3: Capturing SWO Data on Your PC**
Once SWO is set up on the STM32, the **ST-LINK debugger** captures the SWO data and sends it to the PC via USB.

### **🔍 Checking if SWO is Detected**
Run:
```bash
lsusb | grep STMicroelectronics
dmesg | grep tty
```
You should see an ST-LINK device detected.

### **🔧 Using `stlink` Tools to Capture SWO Output**
If you are using the **stlink tools**:
```bash
st-info --probe
st-trace -v -a 115200
```
- `st-trace` captures SWO data in real-time.

### **🔧 Using `OpenOCD` to Capture SWO Output**
Start OpenOCD:
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
```
Then, run `telnet` to capture SWO data:
```bash
telnet localhost 4444
```
Inside telnet:
```
> tpiu config internal /tmp/swo.log uart off 1000000
> itm port 0 on
```
Now, SWO output will be written to `/tmp/swo.log`.

### **🔧 Using GDB to Capture SWO**
If you’re using **GDB with OpenOCD**:
```bash
(gdb) monitor tpiu config internal /tmp/swo.log uart off 1000000
(gdb) monitor itm port 0 on
```
Now, `printf` will send data to `/tmp/swo.log`.

---

## **💡 How Data Flows from STM32 to Your PC**
1. **STM32 sends data to `ITM->STIM[0]`**.
2. **ITM forwards data to the Trace Port Interface Unit (TPIU)**.
3. **TPIU encodes data and sends it over the SWO pin** (PB3).
4. **ST-LINK captures SWO data and sends it over USB**.
5. **Linux driver captures SWO data and makes it available via `/dev/ttyACM0` or OpenOCD.**

---

## **🛠️ Step 4: Using SWO `printf` Instead of UART**
You can override `printf` to use SWO:
```c
#include <stdio.h>

int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        SWO_PrintChar(ptr[i]);
    }
    return len;
}

int main() {
    SWO_Init(115200);
    printf("Hello, SWO!\n");
    while (1);
}
```
This redirects `printf` output to **SWO instead of UART**.

---

## **🚀 Summary**
- **SWO is a one-wire debugging interface for real-time logging.**
- **ITM (Instrumentation Trace Macrocell) is used to send data over SWO.**
- **ST-LINK captures SWO data and sends it to the PC via USB.**
- **Linux drivers and OpenOCD tools capture the SWO output.**
- **You can use `printf` via SWO instead of UART for debugging.**