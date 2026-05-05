**Smart Door Security System (ESP32 + RFID)**



**Overview**

Built an IoT-based smart door security system using ESP32 and RFID authentication, with real-time WhatsApp alerts for unauthorized access. Designed as a low-cost, scalable solution for home and office security.



**Features**

RFID-based access control

Real-time WhatsApp alerts (CallMeBot API)

LCD status display

LED \& buzzer feedback

Wi-Fi connectivity with auto-reconnect



**Tech Stack**

Hardware: ESP32, RC522 RFID, LCD (I2C), LEDs, Buzzer

Software: Arduino (C++)

Networking: Wi-Fi (HTTP)



**System Flow**

Connects to Wi-Fi and waits for RFID input

Authorized UID -> Access Granted (green LED)

Unauthorized UID -> Access Denied (red LED + buzzer)

Sends WhatsApp alert with UID details



**Results**

Consistent real-time alert delivery

Stable Wi-Fi performance

Reliable access control across multiple tests



**Setup**

Configure Wi-Fi + API credentials

Upload code via Arduino IDE

Scan RFID tags to test



**Future Improvements**

Mobile app integration

Cloud logging

Biometric authentication



**Author**

Masanabo M.I

Tshwane University of Technology

