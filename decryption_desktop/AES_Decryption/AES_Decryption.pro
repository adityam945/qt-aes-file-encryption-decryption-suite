/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

TARGET = AES_Decryption

# --- Libsodium Integration ---
# Ensure this path points correctly to your 3rd_party folder relative to this .pro file
INCLUDEPATH += $$PWD/../3rd_party/libsodium/include

# --- Source Files ---
HEADERS += \
    AES_File_Decryption.h \
    DecryptionWorker.h

SOURCES += \
    AES_File_Decryption.cpp \
    DecryptionWorker.cpp
    # Note: main.cpp is intentionally EXCLUDED because this is now a library
