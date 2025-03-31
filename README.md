# jonckheere_remote_control
Remote control for the Jonckheere refractor (From Lille observatory)

The goal is to develop an wireless remote command of the mount of the Jonckheere refractor. A esp32 micro controller will manage the motor's drivers of the mount, and another ESP32 will manage all the buttons to pilot it.

All the communication between the ERP32 are using UDP protocol to ensure fast response. 
For the development purpose, an Android application has been also developped using react native.

![Jonckheere Refractor](./images/interface.png)