
# AES-256 File Encryption Suite

Qt 5.15 AES-256 File Encryption Suite.

A pair of high-performance C++ console applications built with Qt 5.15 and libsodium. This suite allows for recursive directory encryption with filename obfuscation and secure 256-bit key management. (refer workflow)

## Project Tree
    ├── decryption_desktop/          # Project 1: Restores encrypted folders
    ├── encyption_desktop/          # Project 2: Encrypts local directories
    ├── encyption_desktop/          # standalone python script to get machine_id and push to encryption server
---
# Prerequisites

Developed and Tested on Ubuntu 22.04

three main components installed on your system:

    C++ Compiler: A modern compiler supporting C++11 or higher (GCC, Clang, or MSVC).

    Qt Framework: The GUI and Network libraries.

    Libsodium: The cryptography library for AES and RSA-like operations.

Build Libsodium LocallyTo ensure portability and avoid sudo apt install, build libsodium inside the 3rd_party folder:

```bash
# From the root directory
wget https://download.libsodium.org/libsodium/releases/libsodium-1.0.20-stable.tar.gz

tar -xvf libsodium-1.0.20-stable.tar.gz

cd libsodium-stable

./configure --prefix=$(pwd)/../3rd_party/libsodium --disable-shared
make && make install

cd .. && rm -rf libsodium-stable*
```
---
## Testing

```bash
Tested on Ubuntu 22.04 and 20.04 with C++11 (11.4.0)

encryption machine - Ubuntu 22.04 both qt shadow build and docker

decyprtion machine - 
    1. Ubuntu 22.04 qt shadow build;  
    2. Ubuntu 20.04 qt shadow build;

(setup and running successful)
```

---
# Encryption Application (AES_Encryption)

This tool performs a recursive scan of a target folder, generates a single session key, and obfuscates filenames.
---
Project tree

    <path>/encyption_desktop$ tree -L 3
    .
    ├── 3rd_party
    │   └── libsodium
    │       ├── include
    │       └── lib
    ├── AES_Encryption
    │   ├── AES_Encryption.pro
    │   ├── AES_File_encryption.cpp
    │   ├── AES_File_encryption.h
    │   ├── AES_Key_genration.cpp
    │   ├── AES_Key_genration.h
    │   ├── EncryptionWorker.cpp
    │   └── EncryptionWorker.h
    ├── APIManager
    │   ├── APIController.cpp
    │   ├── APIController.h
    │   ├── APIManager.pro
    │   ├── WebManager.cpp
    │   └── WebManager.h
    ├── DBManager
    │   ├── DBmanager.cpp
    │   ├── DBmanager.h
    │   └── DBmanager.pro
    ├── encryption_desktop.pro
    ├── main.cpp
    ├── MainWindow.cpp
    ├── MainWindow.h
    ├── README.md
    └── RSA_Key_Encryption
        ├── RSA_Key_Encryption.cpp
        ├── RSA_Key_Encryption.h
        └── RSA_Key_Encryption.pro

    8 directories, 24 files
---
Build & Run: Encryption Application (Server)

```bash
# Navigate to project root
cd ~/<project_path>/qt-aes-file-encryption-decryption-suite

# 1. Create a shadow build directory
mkdir build-encryption
cd build-encryption

# 2. Run qmake pointing to the source .pro file
qmake ../encyption_desktop/encryption_desktop.pro

# 3. Compile
make

# 4. Run the executable
# (Ensure libsodium is installed or in LD_LIBRARY_PATH)
./Encryption_Desktop
```
For API endpoints see /path/to/project/encyption_desktop/README.md

---

# Decryption Application (AES_Decryption)
This tool reverses the process, reconstructing the original files and folder tree.
Project tree

    <path>/decryption_desktop$ tree -L 3
    .
    ├── 3rd_party
    │   └── libsodium
    │       ├── include
    │       └── lib
    ├── AES_Decryption
    │   ├── AES_Decryption.pro
    │   ├── AES_File_Decryption.cpp
    │   ├── AES_File_Decryption.h
    │   ├── DecryptionWorker.cpp
    │   ├── DecryptionWorker.h
    │   └── main.cpp
    ├── decryption_desktop.pro
    ├── main.cpp
    ├── MainWindow.cpp
    ├── MainWindow.h
    ├── README.md
    └── RSA_Key_Decryption
        ├── RSA_Key_Decryption.cpp
        ├── RSA_Key_Decryption.h
        └── RSA_Key_Decryption.pro

6 directories, 15 files
---
Build & Run: Decryption Application (Client)

```bash
# Navigate to project root
cd ~/<project_path>/qt-aes-file-encryption-decryption-suite

# 1. Create a shadow build directory
mkdir build-decryption
cd build-decryption

# 2. Run qmake pointing to the source .pro file
qmake ../decryption_desktop/decryption_desktop.pro

# 3. Compile
make

# 4. Run the executable
./Decryption_Desktop
```

## Build & Run with Docker (Encryption Server)

You can containerize the Encryption Server application to run it in an isolated environment while still accessing the GUI and persisting data on your host machine.

### Prerequisites

    * Docker: [Install Docker Engine](https://docs.docker.com/engine/install/)
    * Docker Compose: (Optional, for easier management) [Install Docker Compose](https://docs.docker.com/compose/install/)
    * Linux OS: Recommended for native X11 forwarding. (Windows/Mac requires X Server configuration like VcXsrv/XQuartz).

### 1. Quick Start (Docker Compose)
This is the recommended method. It handles building, network settings, and volume mapping automatically.

1.  **Allow GUI Access:**
    ```bash
    xhost +local:docker
    ```

2.  **Start the Server:**
    Run this command from the project root (where `docker-compose.yaml` is located):
    ```bash
    sudo docker-compose up --build
    ```

The application window should appear on your screen. The database is automatically saved to the `./encryption_db_storage` folder on your host machine.

---

### 2. Manual Build (Docker CLI)

If you prefer using standard Docker commands:
```bash
Step 1: Build the Image

# Run from project root
sudo docker build -f docker/Dockerfile -t encryption-app .

Step 2: Prepare Host Data Folder

Bash

mkdir -p ~/encryption_data
Step 3: Run the Container

Bash

xhost +local:docker

sudo docker run -it --rm \
    --net=host \
    --device /dev/dri:/dev/dri \
    -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v ~/encryption_data:/app/encyption_desktop/db \
    encryption-app