# Networks_Subject_Projects

## About

* **Name of the video game:** Space Destroyers 
* **Description:** Space Destroyers is an on-line multiplayer video game in which you control a powerful spaceship, with three kinds of weapons. Your objective is simple: eliminate as many opponents as you can in order to climb the leaderboard and show who is the best!

## Authors

* [Gerard Marcos](https://github.com/vsRushy)

* [Marc Doctor](https://github.com/thedoctormarc)

## Controls

* **Down Arrow:** Throttle

* **A:** Turn Left

* **D:** Turn Right

* **Left Arrow:** Shoot

* **Up Arrow:** Change Weapon

## Contributions

### Client/Server Virtual Connection

* **Contributors:** Gerard and Marc.

* **Achieved:** Completely.

* **Description:** The application supports robustly a system which allows a server creation, and the clients are able to join the server successfully. The server and clients have an ID to identify them, and the system also handles disconnections. Naturally, if the server is shut down, all the clients will be disconnected, for instance.

### World State Replication

* **Contributor:** Gerard.

* **Achieved:** Completely.

* **Description:** The main objective of this subsystem is to replicate the state of the world from the server to all clients. In essence, an unordered map of replication commands, with its correspondant id, is stored and being sent to all clients. Each replication command has three types: Create, Update, and Destroy.

### Delivery Manager

* **Contributor:** Marc.

* **Achieved:** Partially.

* **Description:**
The purpose of the delivery manager is to keep track of network packets in a way that each one has a sequence number assigned so that we can take into account packets drops, give acknowledgments when a packet is received by the server...

### Entity Interpolation

* **Contributors:** Gerard and Marc.

* **Achieved:** Partially.

* **Description:** Instead of sendind the position and angle of the player each time a packet is sent, we can go further and perform an interpolation between these values. Interpolation can be done because we send a packet each X seconds and, thus, we can do an interpolation so that we have a smooth movement each frame. The system is currently disabled as there are some problems regarding the interpolation operations, such as stuttering.

### Gameplay

* **Feature:** Scoreboard. **Contributor:** Marc.

* **Feature:** Player Respawn. **Contributor:** Marc.

* **Feature:** Weapons. **Contributor:** Gerard.

* **Feature:** Player stats. **Contributor:** Gerard.

## Instructions

* **1.** Download the latest release [here](https://github.com/vsRushy/Networks_Subject_Projects/releases)

* **2.** Unzip the file you just downloaded.

* **3.** Execute Networks.exe.

* **4.** Press on the "Start server" button on the "Main Menu" window.

* **5.** Press on the "Connect to server" button on the "Main Menu" window. Don't forget to write your desired username on the textbox. NOTE: you can repeat this step as many clients as you want.

* **Disclaimer:** Don't modify any of the files or otherwise the application might not work properly or not work at all.

## License

MIT License

Copyright (c) 2020 Gerard Marcos and Marc Doctor

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
