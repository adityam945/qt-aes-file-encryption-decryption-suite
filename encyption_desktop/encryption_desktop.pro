TEMPLATE = app
TARGET = Encryption_Desktop
CONFIG += c++11

# --- 1. Qt Modules ---
QT += core gui widgets sql network

# --- 2. Dependencies ---
INCLUDEPATH += $$PWD/3rd_party/libsodium/include
LIBS += -L$$PWD/3rd_party/libsodium/lib -lsodium

# --- 3. Include Paths ---
INCLUDEPATH += \
    $$PWD/AES_Encryption \
    $$PWD/DBManager \
    $$PWD/APIManager \
    $$PWD/RSA_Key_Encryption

# --- 4. Source Files ---
SOURCES += \
    main.cpp \
    MainWindow.cpp \
    AES_Encryption/EncryptionWorker.cpp \
    AES_Encryption/AES_File_encryption.cpp \
    AES_Encryption/AES_Key_genration.cpp \
    DBManager/DBmanager.cpp \
    RSA_Key_Encryption/RSA_Key_Encryption.cpp \
    APIManager/APIController.cpp \
    APIManager/WebManager.cpp
    

# --- 5. Headers ---
HEADERS += \
    MainWindow.h \
    AES_Encryption/EncryptionWorker.h \
    AES_Encryption/AES_File_encryption.h \
    AES_Encryption/AES_Key_genration.h \
    DBManager/DBmanager.h \
    RSA_Key_Encryption/RSA_Key_Encryption.h \
    APIManager/APIController.h \
    APIManager/WebManager.h
    
