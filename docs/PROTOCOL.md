# Binary Game Protocol Specification

## 1. Introduction

This specification defines a simple binary protocol for the R-Type server and its clients. The protocol supports:

- Client connection and disconnection.
- Server state updates (entities and their properties).
- Client input messages (movements, actions).

All messages use a fixed-size header followed by a typed body. Data is transmitted in network byte order.

## 2. Message Format

**Common Message Header (4 bytes):**

```
0               1               2               4  (Bytes)
+-------+-------+-------+-------+
|  Type (16)    |  Size (16)   |
+---------------+---------------+
```

- **Type**: Message type (`1=Connect`, `2=Disconnect`, `3=StateUpdate`, `4=UserInput`)
- **Size**: Total message size in bytes (header + body).

All multi-byte fields are in network byte order.

## 3. Message Types

### 3.1 Connect (Type = 1)

**Format:**

```
Header (Type=1, Size=8)
Body:
+-------------------------------+
|          ClientId (32)        |
+-------------------------------+
```

Used by clients to request a connection and by servers to acknowledge and assign the `ClientId`.

### 3.2 Disconnect (Type = 2)

**Format:**

```
Header (Type=2, Size=8)
Body:
+-------------------------------+
|          ClientId (32)        |
+-------------------------------+
```

Indicates a client is disconnecting or the server acknowledges disconnection.

### 3.3 StateUpdate (Type = 3)

**Format:**

```
Header (Type=3, Size=4 + 4 + N*23)
Body:
+-------------------------------+
|        NumEntities (32)       |
+-------------------------------+
|     EntityState[0..N-1]       |
+-------------------------------+
```

**EntityState (per entity):**

```
Total Size: 23 bytes 
EntityId (32), PosX (float32), PosY (float32), VelX (float32), VelY (float32), EntityType (16), Health (8)
```

Sent by the server to all clients, describing the current game state.

### 3.4 UserInput (Type = 4)

**Format:**

```
Header (Type=4, Size=9)
Body:
+-------------------------------+
|          ClientId (32)        |
+---------------+---------------+
|   InputFlags (8)              |
+-------------------------------+
```

**InputFlags** is a bitfield:

- Bit 0: MoveUp
- Bit 1: MoveDown
- Bit 2: MoveLeft
- Bit 3: MoveRight
- Bit 4: Fire

Multiple inputs can be combined (e.g., MoveUp|Fire), (And on server we get all the active bits with & operator).

```c++
// example server
    bool moveUp = (inputMsg.inputFlags & static_cast<uint8_t>(InputFlags::MoveUp)) != 0;
    bool moveDown = (inputMsg.inputFlags & static_cast<uint8_t>(InputFlags::MoveDown)) != 0;
    bool moveLeft = (inputMsg.inputFlags & static_cast<uint8_t>(InputFlags::MoveLeft)) != 0;
    bool moveRight = (inputMsg.inputFlags & static_cast<uint8_t>(InputFlags::MoveRight)) != 0;
    bool fire = (inputMsg.inputFlags & static_cast<uint8_t>(InputFlags::Fire)) != 0;
```

