### **Virtual COM Port (VCOM) on Nucleo STM32 Boards – A Detailed Explanation**  

---

## **1. What is VCOM?**  
VCOM (Virtual COM Port) is a **USB-to-serial bridge** that allows an STM32 microcontroller to communicate with a PC **via the onboard ST-LINK debugger**. This enables the STM32 to send and receive UART (serial) data over USB without needing an external USB-to-UART adapter.  

In simple terms, **VCOM makes your STM32 behave as if it were connected to your PC using a traditional UART-to-USB converter (like an FTDI module), but instead, it uses the ST-LINK interface.**  

---

## **2. How Does VCOM Work?**  

### **2.1. Physical Connections**  
A Nucleo board has two main components:  
1. **STM32 Microcontroller** – The actual MCU where your program runs.  
2. **ST-LINK Debugger Chip** – Provides debugging, programming, and VCOM support.  

The ST-LINK debugger has a **built-in USB-to-UART bridge**, which is internally connected to the STM32's UART pins.  

For most **Nucleo-F4/F3/F1** series boards:  
- **TX (Transmit) → STM32 USART2 TX (PA2)**  
- **RX (Receive) → STM32 USART2 RX (PA3)**  

This means that any **data sent by the STM32’s USART2** is captured by ST-LINK and forwarded to the PC **as USB data**.  

---

### **2.2. Software Representation**  

When you connect a Nucleo board to Ubuntu (or any OS), the ST-LINK interface exposes multiple communication channels:  

| **Function**           | **Purpose**                          | **Linux Device**         |
|------------------------|--------------------------------------|--------------------------|
| **Virtual COM Port**   | USB-UART communication (VCOM)       | `/dev/ttyACM0`           |
| **SWD Debugging**      | Debugging via ST-LINK               | `/dev/hidrawX` (GDB)     |
| **Mass Storage**       | (Optional) Drag-and-drop flashing   | `/media/NODE_F401RE`     |

The **VCOM interface is represented as `/dev/ttyACM0` on Linux** (or `COMx` on Windows). This behaves like a normal serial port, allowing bidirectional communication.  

---

### **2.3. Data Flow in VCOM**  
**Sending Data from STM32 to PC:**  
1. The STM32 writes data to **USART2 TX (PA2)** using `HAL_UART_Transmit()`.  
2. The ST-LINK chip reads this data and forwards it over USB.  
3. The PC receives the data through `/dev/ttyACM0`.  
4. A terminal program (`screen`, `minicom`, etc.) can read and display the data.  

**Sending Data from PC to STM32:**  
1. The user sends data using `echo "Hello" > /dev/ttyACM0`.  
2. The ST-LINK chip receives the USB data and forwards it to **USART2 RX (PA3)**.  
3. The STM32 reads this data using `HAL_UART_Receive()`.  

---

## **3. How to Use VCOM on Ubuntu**  

### **3.1. Checking if VCOM is Available**  
After connecting your Nucleo via USB, run:  
```sh
ls /dev/ttyACM*
```  
If VCOM is active, you should see:  
```
/dev/ttyACM0
```

If not, check permissions:  
```sh
sudo usermod -a -G dialout $USER
```
(Log out and back in for changes to take effect.)  

---

### **3.2. Opening the Serial Port**  
To read data from STM32, use:  
```sh
screen /dev/ttyACM0 115200
```
(Replace `115200` with your actual baud rate.)  

To exit `screen`:  
```
Ctrl + A, then K, then Y
```

To write data from PC to STM32:  
```sh
echo "Hello STM32" > /dev/ttyACM0
```

---

### **3.3. Configuring VCOM on STM32**  
If your firmware doesn’t output anything, ensure USART2 is enabled in your code.  

#### **Example Code (Bare Metal HAL)**
```c
#include "stm32f4xx_hal.h"
UART_HandleTypeDef huart2;

void UART2_Init(void) {
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;  // PA2=TX, PA3=RX
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart2);
}

void send_message(char *msg) {
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

int main(void) {
    HAL_Init();
    UART2_Init();

    while (1) {
        send_message("Hello from STM32!\r\n");
        HAL_Delay(1000);
    }
}
```
After flashing this firmware, you should see "Hello from STM32!" appearing in `screen`.  

---

## **4. Common Questions and Misconceptions**  

### **4.1. Does All ST-LINK Data Go Through `/dev/ttyACM0`?**  
**No.** `/dev/ttyACM0` is only for **VCOM UART communication**. ST-LINK debugging (GDB, OpenOCD) uses a separate USB HID interface (`/dev/hidrawX`).  

### **4.2. Can I Use VCOM and SWD Debugging Simultaneously?**  
Yes. VCOM and debugging operate independently over the same USB connection. You can:  
- Debug your STM32 using GDB/OpenOCD.  
- Read logs from VCOM in a separate terminal.  

### **4.3. Why Can’t I See `/dev/ttyACM0`?**  
Possible reasons:  
1. **ST-LINK firmware is outdated** → Update using STM32CubeProgrammer.  
2. **STM32 firmware doesn’t enable USART2** → Check your code.  
3. **Wrong permissions** → Add user to `dialout` group (`sudo usermod -a -G dialout $USER`).  

### **4.4. Does VCOM Work on Custom STM32 Boards?**  
No, unless you manually add a USB-to-UART bridge (FTDI, CH340, or similar). VCOM is only available on Nucleo boards because it depends on the **ST-LINK chip**, which is built into the board.  

---

## **5. Summary**  

✅ **VCOM = Virtual Serial Port via ST-LINK (USB-to-UART bridge)**  
✅ **Appears as `/dev/ttyACM0` on Ubuntu**  
✅ **Only carries UART data; debugging uses a different USB interface**  
✅ **Uses STM32’s USART2 (PA2=TX, PA3=RX on most Nucleo boards)**  
✅ **Useful for debugging logs, command input, and data exchange**  
✅ **Can be used alongside ST-LINK debugging without conflicts**  

### **Virtual COM Port (VCOM) on Nucleo STM32 Boards – Bare Metal Explanation**  

---

## **1. What is VCOM?**  
VCOM (**Virtual COM Port**) is a **USB-to-UART bridge** built into Nucleo boards via the **ST-LINK debugger chip**. It allows an STM32 microcontroller to communicate with a PC using a virtual serial port over USB.  

In simple terms:  
✅ **Data sent over USB from the PC is first converted into UART** and sent to the STM32.  
✅ **Data sent from the STM32 over UART is received by ST-LINK and converted back to USB**, appearing as `/dev/ttyACM0` (Linux) or `COMx` (Windows).  

This eliminates the need for an external **USB-to-UART adapter (FTDI, CH340, etc.)**, as the ST-LINK chip handles everything.  

---

## **2. How Does VCOM Work?**  

### **2.1. Hardware Connections**  

The **ST-LINK debugger chip** on a Nucleo board is internally connected to the STM32's UART pins:  

| **STM32 Pin**  | **Function** | **ST-LINK Connection** |
|---------------|-------------|------------------------|
| PA2          | USART2 TX   | USB (sent to PC)       |
| PA3          | USART2 RX   | USB (received from PC) |

Thus, any data sent from the STM32’s **USART2 TX (PA2)** is forwarded by ST-LINK over USB. Similarly, data received over USB is forwarded to **USART2 RX (PA3)**.  

---

### **2.2. Data Flow in VCOM**  

#### **Sending Data (STM32 → PC)**
1. The STM32 writes data to **USART2 TX (PA2)**.  
2. The ST-LINK debugger reads the UART data.  
3. The ST-LINK chip **encapsulates it into a USB CDC (Communications Device Class) packet**.  
4. The PC receives it via `/dev/ttyACM0` as a standard serial input.  

#### **Receiving Data (PC → STM32)**
1. The user sends data using `echo "Hello" > /dev/ttyACM0`.  
2. The PC **encodes it as a USB CDC packet** and sends it to the ST-LINK debugger.  
3. The ST-LINK chip extracts the data and forwards it to **USART2 RX (PA3)**.  
4. The STM32 reads the data from its UART RX register.  

This means **data is always converted between USB and UART**, allowing seamless serial communication.  

---

## **3. Using VCOM on Ubuntu (Bare Metal)**  

### **3.1. Checking if VCOM is Available**  
After connecting your Nucleo via USB, run:  
```sh
ls /dev/ttyACM*
```
If VCOM is working, you should see:  
```
/dev/ttyACM0
```
If not, fix permissions:  
```sh
sudo usermod -a -G dialout $USER
```
(Log out and back in for changes to take effect.)  

---

### **3.2. Opening and Sending Data Over VCOM**  
To **view STM32 output** in real time:  
```sh
screen /dev/ttyACM0 115200
```
To **send data from PC to STM32**:  
```sh
echo "Hello STM32" > /dev/ttyACM0
```

---

## **4. Configuring VCOM in Bare Metal STM32 Code**  

Since **ST-LINK forwards USART2 data**, we must configure USART2 **manually without HAL**.  

### **4.1. Enabling USART2 in Bare Metal**  
This involves:  
1. **Enabling GPIOA (PA2, PA3) for alternate function mode**.  
2. **Configuring USART2 registers for 115200 baud, 8N1 mode**.  
3. **Implementing basic UART transmit and receive functions**.  

#### **Bare Metal Code (STM32F4)**
```c
#include "stm32f4xx.h"

void USART2_Init(void) {
    // Enable clocks for USART2 and GPIOA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // Configure PA2 (TX) and PA3 (RX) as alternate function
    GPIOA->MODER |= (2 << (2 * 2)) | (2 << (3 * 2)); // AF mode
    GPIOA->AFR[0] |= (7 << (4 * 2)) | (7 << (4 * 3)); // AF7 (USART2)

    // Configure USART2: 115200 baud, 8N1 mode
    USART2->BRR = (SystemCoreClock / 2) / 115200;  // Baud rate calculation
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;    // Enable TX and RX
    USART2->CR1 |= USART_CR1_UE;                   // Enable USART2
}

void USART2_Write(char ch) {
    while (!(USART2->SR & USART_SR_TXE)); // Wait for TX buffer empty
    USART2->DR = ch;
}

char USART2_Read(void) {
    while (!(USART2->SR & USART_SR_RXNE)); // Wait for RX buffer full
    return USART2->DR;
}

void USART2_Print(char *str) {
    while (*str) USART2_Write(*str++);
}

int main(void) {
    USART2_Init();
    while (1) {
        USART2_Print("Hello from STM32!\r\n");
        for (volatile int i = 0; i < 1000000; i++); // Delay
    }
}
```
**Explanation:**  
- This code **initializes USART2 manually** and sends "Hello from STM32!" every second.  
- You should see this message appear when running `screen /dev/ttyACM0 115200`.  
- Data can be received using `USART2_Read()`.  

---

## **5. Common Questions & Doubts**  

### **5.1. Is `/dev/ttyACM0` Used for Debugging?**  
**No.** `/dev/ttyACM0` is **only for UART communication**. Debugging is done through a separate USB HID interface (`/dev/hidrawX`).  

### **5.2. Can I Use VCOM and Debugging at the Same Time?**  
Yes. ST-LINK can handle:  
✅ **SWD debugging (for GDB)**  
✅ **VCOM UART communication (for logs and serial input)**  
simultaneously.  

### **5.3. Why is `/dev/ttyACM0` Not Appearing?**  
Possible reasons:  
1. **ST-LINK firmware is outdated** → Update it using STM32CubeProgrammer.  
2. **USART2 is not initialized** → Ensure PA2/PA3 are configured correctly.  
3. **Wrong permissions** → Run `sudo usermod -a -G dialout $USER`.  

### **5.4. Does VCOM Work on Custom STM32 Boards?**  
**No,** unless you add an external USB-to-UART converter (FTDI, CH340, etc.). VCOM is only available on **Nucleo boards** because it relies on the ST-LINK debugger chip.  

---

## **6. Summary (Technical and Practical Understanding)**  

### ✅ **What is VCOM?**  
- A **USB-to-UART bridge** inside the ST-LINK debugger.  
- Converts USB CDC data into STM32 UART data (and vice versa).  

### ✅ **How Does Data Flow?**  
- **PC to STM32** → Data over `/dev/ttyACM0` is converted to UART (PA3).  
- **STM32 to PC** → UART data (PA2) is converted to USB and sent over `/dev/ttyACM0`.  

### ✅ **Why is it Useful?**  
- Eliminates the need for an **external FTDI adapter**.  
- Enables **logging, debugging, and serial communication** over USB.  

### ✅ **How to Use it in Bare Metal?**  
- **Manually configure USART2 (PA2, PA3)**.  
- **Enable UART transmission and reception**.  
- **Use `/dev/ttyACM0` in Linux for serial communication**.  

---