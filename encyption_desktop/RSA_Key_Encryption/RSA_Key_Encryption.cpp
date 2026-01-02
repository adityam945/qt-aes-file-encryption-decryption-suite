/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "RSA_Key_Encryption.h"
#include <QFile>
#include <QDebug>
#include <sodium.h>

bool RSAKeyEncryption::wrapKey(const QString &inputKeyPath, const QString &machinePublicKeyHex, QString &outputKeyPath) {
    if (sodium_init() < 0) return false;

    // Read the AES Key 
    QFile inputFile(inputKeyPath);
    if (!inputFile.open(QIODevice::ReadOnly)) {
        qCritical() << "RSA: Cannot open input key file:" << inputKeyPath;
        return false;
    }
    QByteArray hexAesKey = inputFile.readAll().trimmed();
    inputFile.close();
    
    // Convert Hex Key back to Binary for encryption
    QByteArray binAesKey = QByteArray::fromHex(hexAesKey);

    // Parse Key
    QByteArray binPubKey = QByteArray::fromHex(machinePublicKeyHex.toUtf8());
    if (binPubKey.size() != crypto_box_PUBLICKEYBYTES) {
        qCritical() << "RSA: Invalid Public Key size.";
        return false;
    }

    // Encrypt
    // Output size = Message Size + SEALBYTES overhead
    std::vector<unsigned char> ciphertext(crypto_box_SEALBYTES + binAesKey.size());
    
    if (crypto_box_seal(ciphertext.data(), 
                        reinterpret_cast<const unsigned char*>(binAesKey.data()), 
                        binAesKey.size(), 
                        reinterpret_cast<const unsigned char*>(binPubKey.data())) != 0) {
        qCritical() << "RSA: Encryption failed.";
        return false;
    }

    
    // Save Key
    // We usually append .rsa or .enc_rsa to the filename
    outputKeyPath = inputKeyPath + ".rsa_locked";
    QFile outputFile(outputKeyPath);
    if (outputFile.open(QIODevice::WriteOnly)) {
        // Save as Hex for easier handling/transfer
        QByteArray hexCipher = QByteArray(reinterpret_cast<const char*>(ciphertext.data()), ciphertext.size()).toHex();
        outputFile.write(hexCipher);
        outputFile.close();
        
        qDebug() << "RSA: Key wrapped successfully to:" << outputKeyPath;
        
        // QFile::remove(inputKeyPath); 
        return true;
    }

    return false;
}