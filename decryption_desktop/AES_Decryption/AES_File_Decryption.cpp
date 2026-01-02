/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "AES_File_Decryption.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <sodium.h>

const qint64 CHUNK_SIZE = 64 * 1024 * 1024;
const int MAC_BYTES = crypto_secretbox_MACBYTES; 

bool AESFileDecryption::decryptFile(const QString &encFilePath, 
                                   const std::vector<unsigned char> &rawKey, 
                                   const QString &outputRootDir,
                                   std::function<void(int)> progressCallback) {
    if (sodium_init() < 0) return false;

    QFile encFile(encFilePath);
    if (!encFile.open(QIODevice::ReadOnly)) return false;

    qint64 totalSize = encFile.size();
    qint64 bytesRead = 0;

    if (totalSize < crypto_secretbox_NONCEBYTES) return false;
    QByteArray baseNonceData = encFile.read(crypto_secretbox_NONCEBYTES);
    bytesRead += baseNonceData.size();
    std::vector<unsigned char> baseNonce(baseNonceData.begin(), baseNonceData.end());

    QFileInfo encInfo(encFilePath);
    QByteArray hexName = encInfo.fileName().toUtf8();
    QByteArray obfuscatedPath = QByteArray::fromHex(hexName);
    QByteArray originalPathBytes;
    
    // XOR Recovery
    for(int i = 0; i < obfuscatedPath.size(); ++i) {
        originalPathBytes.append(obfuscatedPath[i] ^ rawKey[i % rawKey.size()]);
    }
    QString originalRelPath = QString::fromUtf8(originalPathBytes);
    
    QString fullOutPath = outputRootDir + "/" + originalRelPath;
    QDir().mkpath(QFileInfo(fullOutPath).absolutePath());
    
    QFile outFile(fullOutPath);
    if (!outFile.open(QIODevice::WriteOnly)) {
        encFile.close();
        return false;
    }

    // Chunk Loop
    int blockIndex = 0;
    // We read chunks of (64MB + 16 bytes overhead)
    qint64 readChunkSize = CHUNK_SIZE + MAC_BYTES; 

    while (!encFile.atEnd()) {
        QByteArray chunk = encFile.read(readChunkSize);
        if (chunk.size() <= MAC_BYTES) break; 

        std::vector<unsigned char> currentNonce = baseNonce;
        for (int i = 0; i < 4; i++) {
            int val = currentNonce[crypto_secretbox_NONCEBYTES - 1 - i] + ((blockIndex >> (8*i)) & 0xFF);
            currentNonce[crypto_secretbox_NONCEBYTES - 1 - i] = static_cast<unsigned char>(val);
        }

        // Decrypt
        std::vector<unsigned char> decrypted(chunk.size() - MAC_BYTES);
        
        if (crypto_secretbox_open_easy(decrypted.data(), 
                                       reinterpret_cast<const unsigned char*>(chunk.data()), 
                                       chunk.size(), 
                                       currentNonce.data(), 
                                       rawKey.data()) != 0) {
            qCritical() << "Decryption Error: MAC check failed for chunk" << blockIndex;
            outFile.remove(); 
            return false;
        }

        outFile.write(reinterpret_cast<const char*>(decrypted.data()), decrypted.size());
        
        bytesRead += chunk.size();
        blockIndex++;

        // Progress
        if (progressCallback && totalSize > 0) {
            int percent = static_cast<int>((bytesRead * 100) / totalSize);
            progressCallback(percent);
        }
    }

    outFile.close();
    encFile.close();
    return true;
}