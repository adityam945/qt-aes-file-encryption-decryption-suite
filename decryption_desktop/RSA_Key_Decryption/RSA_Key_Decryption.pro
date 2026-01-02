/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

TARGET = RSA_Key_Decryption

# --- Libsodium Integration ---
INCLUDEPATH += $$PWD/../3rd_party/libsodium/include

# --- Source Files ---
HEADERS += \
    RSA_Key_Decryption.h

SOURCES += \
    RSA_Key_Decryption.cpp
    # Note: main.cpp is intentionally EXCLUDED
