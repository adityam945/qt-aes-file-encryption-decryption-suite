/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "AES_File_encryption.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QDateTime>
#include <QDebug>
#include <sodium.h>

const qint64 CHUNK_SIZE = 64 * 1024 * 1024; 

bool AESFileEncryption::encryptFile(const QString &filePath, 
                                   const QString &baseInputDir, 
                                   const QString &targetEncFolder, 
                                   const std::vector<unsigned char> &key,
                                   std::function<void(int)> progressCallback) {
    QFile inputFile(filePath);
    if (!inputFile.open(QIODevice::ReadOnly)) return false;

    // Setup Paths & Obfuscate Name (Same as before)
    QDir baseDir(baseInputDir);
    QString relativePath = baseDir.relativeFilePath(filePath);
    QByteArray pathBytes = relativePath.toUtf8();
    QByteArray obfuscatedPath;
    for(int i = 0; i < pathBytes.size(); ++i) obfuscatedPath.append(pathBytes[i] ^ key[i % key.size()]);
    QString hexName = obfuscatedPath.toHex();
    
    QFile encFile(targetEncFolder + "/" + hexName);
    if (!encFile.open(QIODevice::WriteOnly)) return false;

    // Generate Base Nonce
    std::vector<unsigned char> baseNonce(crypto_secretbox_NONCEBYTES);
    randombytes_buf(baseNonce.data(), baseNonce.size());
    
    // Write Base Nonce to header ONCE
    encFile.write(reinterpret_cast<const char*>(baseNonce.data()), baseNonce.size());

    // Chunk Loop
    qint64 totalSize = inputFile.size();
    qint64 bytesWritten = 0;
    int blockIndex = 0;
    
    while (!inputFile.atEnd()) {
        QByteArray chunk = inputFile.read(CHUNK_SIZE);
        
        // Calculate Nonce for this block (Base + BlockIndex)
        // We simulate a "Big Endian" increment on the last 4 bytes of the nonce
        std::vector<unsigned char> currentNonce = baseNonce;
        int carry = 0;
        /* Simple increment logic for the nonce to ensure uniqueness per chunk */
        for (int i = 0; i < 4; i++) {
            int val = currentNonce[crypto_secretbox_NONCEBYTES - 1 - i] + ((blockIndex >> (8*i)) & 0xFF);
            currentNonce[crypto_secretbox_NONCEBYTES - 1 - i] = static_cast<unsigned char>(val);
        }

        // Encrypt Chunk
        std::vector<unsigned char> ciphertext(crypto_secretbox_MACBYTES + chunk.size());
        crypto_secretbox_easy(ciphertext.data(), 
                              reinterpret_cast<const unsigned char*>(chunk.data()), 
                              chunk.size(), 
                              currentNonce.data(), 
                              key.data());

        // Write to disk
        encFile.write(reinterpret_cast<const char*>(ciphertext.data()), ciphertext.size());
        
        bytesWritten += chunk.size();
        blockIndex++;

        // Report Progress
        if (progressCallback && totalSize > 0) {
            int percent = static_cast<int>((bytesWritten * 100) / totalSize);
            progressCallback(percent);
        }
    }

    inputFile.close();
    encFile.close();
    return true;
}