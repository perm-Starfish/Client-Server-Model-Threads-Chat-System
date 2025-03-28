# Chat-System-Client-Server-Model-Threads-
> **Operating Systems - Assignment 1**  
> Implement a multi-client chat system using UNIX socket programming and Pthreads.  

[![OS Project](https://img.shields.io/badge/Operating%20System-Networking-blue.svg)](https://github.com/)  
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)  

## 📌 **Project Overview**
This project implements a **multi-client chat system** using the **client-server model** with **UNIX sockets** and **Pthreads**. The server maintains user connections, message forwarding, and synchronization using a **shared whiteboard (memory space)**. Clients can send and receive messages in real-time.

---

## 🌟 **Features**
✅ Support **multiple clients** to connect to the server.  
✅ **User login/logout notifications** for all online users.  
✅ **Direct messaging** between clients via the server.  
✅ **Shared whiteboard** for message exchange with synchronization (mutex).  
✅ Implemented using **UNIX sockets (TCP) + Pthreads** for concurrent client handling.  
✅ Simple command-line interface (CLI) for interaction.  

---

## 🛠 **System Architecture**
The system follows a **client-server** architecture:

```
   +---------+       +---------+       +---------+
   | Client1 |       | Client2 |       | Client3 |
   +----+----+       +----+----+       +----+----+
        |                  |                 |
        |   (TCP Socket)   |   (TCP Socket)  |
        |                  |                 |
        v                  v                 v
   +---------------------------------------------+
   |               Chat Server                   |
   |   - Manages user connections                |
   |   - Stores username & socket mappings      |
   |   - Handles message forwarding             |
   |   - Uses shared whiteboard & mutex         |
   +---------------------------------------------+

```

---

## 💻 **Environment & Requirements**
- **OS:** Linux / macOS (recommended), Windows (via WSL or MinGW)
- **Compiler:** `gcc`
- **Libraries:** `pthread`, `sys/socket.h`, `netinet/in.h`

---

## 🚀 **Installation & Usage**

### **1️⃣ Compile the Project**
Use the provided `Makefile` to compile both the server and client programs:
```sh
make
```

This will generate two executable files:
- server
- client

### **2️⃣ Run the Server**
Start the server using the following command:

```sh
./server
```
By default, the server runs on port 1234.

### **3️⃣ Run Clients**
Start multiple clients to connect to the server:

```sh
./client 127.0.0.1 1234 Alice
./client 127.0.0.1 1234 Bob
```
Each client should specify:

```sh
./client <server_ip> <port> <username>
```

For example, to connect as "Charlie":

```sh
./client 192.168.1.100 1234 Charlie
```

## 📝 **Client Commands**

Command	Description
chat <username> "<message>"	Send a message to another user
bye	Disconnect from the server

```sh
(Alice connects)
$ ./client 127.0.0.1 1234 Alice
<User Alice is online.>

(Bob connects)
$ ./client 127.0.0.1 1234 Bob
<User Bob is online.>

(Alice sends a message)
$ chat Bob "Hello Bob!"
<To Bob> Hello Bob.

(Bob leaves)
$ bye
<User Bob is offline.>
```

## 🛠 **Code Structure**

```bash
/chat-system
│── server.c       # Server-side logic
│── client.c       # Client-side logic
│── Makefile       # Compilation script
│── README.md      # Project documentation
│── LICENSE        # License file
```
