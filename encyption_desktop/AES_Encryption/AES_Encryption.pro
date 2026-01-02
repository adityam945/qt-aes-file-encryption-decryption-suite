/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
TEMPLATE = lib
CONFIG += staticlib c++11
TARGET = AES_Encryption

INCLUDEPATH += $$PWD/../3rd_party/libsodium/include

SOURCES += \
EncryptionWorker.cpp \
    AES_File_encryption.cpp \
    AES_Key_genration.cpp
    

HEADERS += \
    EncryptionWorker.h \
    AES_File_encryption.h \
    AES_Key_genration.h 