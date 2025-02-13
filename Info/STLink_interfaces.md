### **Interfaces Exposed by ST-LINK on Nucleo Boards**  

The **ST-LINK debugger** on Nucleo boards acts as a **multi-interface USB device**, exposing multiple functionalities to the PC. It presents itself as a **composite USB device**, meaning that a single physical connection (USB) provides multiple logical interfaces.  

---

## **1. Interfaces Exposed by ST-LINK**  

When you connect a Nucleo board to your PC via USB, the ST-LINK exposes **three (sometimes four) USB interfaces**:

| **Interface**         | **Function**                          | **Linux Device Name** |
|----------------------|--------------------------------|------------------|
| **1. SWD Debugging** | For flashing firmware and debugging via OpenOCD, STM32CubeProgrammer, or GDB. | `/dev/hidrawX` |
| **2. Virtual COM (VCOM)** | Provides a USB-to-UART bridge between PC and STM32. | `/dev/ttyACM0` |
| **3. Mass Storage** | Allows drag-and-drop flashing (like a USB drive). | `/dev/sdX` |
| **4. (Optional) Bridge GPIO/UART/SWD** | Used for additional communication over USB (only on some ST-LINK versions). | `/dev/hidrawX` |

---

## **2. How These Interfaces Work**  

### **1. SWD Debugging Interface** (`/dev/hidrawX`)  
- Used by **GDB, OpenOCD, or STM32CubeProgrammer** to program and debug the STM32 via **SWD (Serial Wire Debug)**.  
- This interface is usually presented as a **USB HID device** (`/dev/hidrawX`) rather than a serial port.  
- Allows **flashing, memory inspection, and real-time debugging**.  

#### **How to Use It:**  
```sh
st-info --probe         # Check if ST-LINK is detected
openocd -f board/st_nucleo_f4.cfg  # Start OpenOCD
arm-none-eabi-gdb       # Debugging
```

---

### **2. Virtual COM Port (VCOM)** (`/dev/ttyACM0`)  
- **USB-to-UART bridge** between the PC and STM32.  
- Acts like a standard **serial port**, but data is converted between USB and STM32 UART (PA2, PA3).  
- Used for **logging, command interfaces, or communication between PC and STM32**.  

#### **How to Use It:**  
To receive STM32 logs:  
```sh
screen /dev/ttyACM0 115200
```
To send data from PC to STM32:  
```sh
echo "Hello STM32" > /dev/ttyACM0
```

---

### **3. Mass Storage Interface** (`/dev/sdX`)  
- **Drag-and-drop firmware flashing** (works like a USB drive).  
- Allows copying `.bin` files directly to the Nucleo board, automatically flashing the firmware.  
- Only works on **Nucleo boards** (not standalone ST-LINK programmers).  

#### **How to Use It:**  
Simply copy a binary firmware file to the mounted Nucleo USB drive:  
```sh
cp firmware.bin /media/$USER/NODE_F401RE/
```
This method is **not as reliable** as flashing via OpenOCD or STM32CubeProgrammer.

---

### **4. Bridge Interface (Optional) (`/dev/hidrawX`)**  
- Some ST-LINK versions expose an additional **GPIO/UART/SWD bridge** interface.  
- Can be used for **custom serial communication** beyond just VCOM.  
- This is rarely used outside of advanced debugging scenarios.  

#### **How to Check If Your ST-LINK Exposes These Interfaces?**  
Run:  
```sh
lsusb -v | grep -i "st-link" -A 20
```
This will list all available USB interfaces provided by ST-LINK.

---

## **3. Summary – How Data is Routed Through These Interfaces**  

- **PC → `/dev/ttyACM0` (VCOM) → ST-LINK → UART (PA2, PA3) → STM32**  
- **PC → `/dev/hidrawX` (SWD) → ST-LINK → SWD Pins (CLK/DIO) → STM32 (for debugging)**  
- **PC → `/dev/sdX` (Mass Storage) → ST-LINK → Flashes firmware to STM32**  

