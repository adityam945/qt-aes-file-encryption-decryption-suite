TEMPLATE = app
TARGET = Decryption_Desktop
CONFIG += c++11

# --- Qt Modules ---
QT += core gui widgets network

# --- Dependencies ---
INCLUDEPATH += $$PWD/3rd_party/libsodium/include
LIBS += -L$$PWD/3rd_party/libsodium/lib -lsodium

# --- Include Paths ---
INCLUDEPATH += \
    $$PWD/AES_Decryption \
    $$PWD/RSA_Key_Decryption

# --- Source Files ---
SOURCES += \
    main.cpp \
    MainWindow.cpp \
    AES_Decryption/AES_File_Decryption.cpp \
    AES_Decryption/DecryptionWorker.cpp \
    RSA_Key_Decryption/RSA_Key_Decryption.cpp 

# --- Headers ---
HEADERS += \
    MainWindow.h \
    AES_Decryption/AES_File_Decryption.h \
    AES_Decryption/DecryptionWorker.h \
    RSA_Key_Decryption/RSA_Key_Decryption.h
