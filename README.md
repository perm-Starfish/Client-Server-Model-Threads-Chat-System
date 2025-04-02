# Client-Server-Model-Threads-Chat-System
> **Operating Systems - Assignment 1**  
> Implement a multi-client chat system using UNIX socket programming and Pthreads.  


##  **Project Overview**
This project implements a **multi-client chat system** using the **client-server model** with **UNIX sockets** and **Pthreads**. The server maintains user connections, message forwarding, and synchronization using a **shared whiteboard (memory space)**. Clients can send and receive messages in real-time.

---

##  **Features**
- Multiple clients can connect to the server.
- Clients can send messages to all users.
- Clients can send private messages to specific users.
- Users are notified when someone joins or leaves the chat.

---

##  **System Architecture**
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
   |   - Stores username & socket mappings       |
   |   - Handles message forwarding              |
   |   - Uses shared whiteboard & mutex          |
   +---------------------------------------------+

```

---

##  **Installation & Usage**

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

##  **Client Commands**

| Command | Description |
| :------ | :---------- |
| `chat <username> "<message>"` | Send a message to another user |
| `bye` | Disconnect from the server |

##  **Implementation Details**
- **Server Side**
   - Creates a TCP socket and binds it to port.
   - Listens for incoming connections.
   - Accepts new clients and assigns them a slot.
   - Handles client messages using separate threads.
   - Supports broadcasting messages and private messaging.
   - Uses **mutex locks** to prevent race conditions.

- **Client Side**
  - Establishes a connection to the server using **`connect()`**.
  - Starts a thread to receive messages.
  - Can exit the chat by sending `bye`.

---

##  **Demo Example**

```pgsql
Terminal 1 (Server):
$ ./server
[Server] Listening on port 1234...
[Server] Alice connected (192.168.1.10)
[Server] Bob connected (192.168.1.11)
[Server] Message from Alice to Bob: "Hello Bob!"
[Server] Bob disconnected.

Terminal 2 (Alice - Client):
$ ./client 127.0.0.1 1234 Alice
<User Alice is online.>
$ chat Bob "Hello Bob!"
<To Bob> Hello Bob.

Terminal 3 (Bob - Client):
$ ./client 127.0.0.1 1234 Bob
<User Bob is online.>
(Alice sent a message)
<Message from Alice: "Hello Bob!">
$ bye
<User Bob is offline.>
```


---

##  **Troubleshooting**
 *Port already in use?*  
 -> Run `netstat -tulnp | grep 1234` and kill the process using `kill -9 <PID>`.  

 *Compilation error?*  
 -> Ensure you have `gcc` and required libraries installed (`sudo apt install build-essential`).  

