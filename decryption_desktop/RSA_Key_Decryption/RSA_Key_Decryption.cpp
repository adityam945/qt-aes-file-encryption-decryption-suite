/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "RSA_Key_Decryption.h"
#include <QFile>
#include <QDebug>
#include <sodium.h>

std::vector<unsigned char> RSAKeyDecryption::unwrapKey(const QString &lockedKeyPath, const QString &privateKeyPath) {
    if (sodium_init() < 0) {
        qCritical() << "Sodium Init Failed";
        return {};
    }

    // 1. Read Private Key (32 bytes raw)
    QFile privFile(privateKeyPath);
    if (!privFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Cannot open Private Key:" << privateKeyPath;
        return {};
    }
    QByteArray privKeyData = privFile.readAll();
    privFile.close();

    if (privKeyData.size() != crypto_box_SECRETKEYBYTES) {
        qCritical() << "Invalid Private Key size. Expected 32 bytes, got" << privKeyData.size();
        return {};
    }

    // 2. Derive Public Key from Private Key
    std::vector<unsigned char> pubKey(crypto_box_PUBLICKEYBYTES);
    crypto_scalarmult_base(pubKey.data(), reinterpret_cast<const unsigned char*>(privKeyData.data()));

    // 3. Read Locked Session Key
    QFile lockedFile(lockedKeyPath);
    if (!lockedFile.open(QIODevice::ReadOnly)) {
        qCritical() << "Cannot open Locked Key:" << lockedKeyPath;
        return {};
    }
    QByteArray hexCipher = lockedFile.readAll().trimmed();
    QByteArray binCipher = QByteArray::fromHex(hexCipher);
    lockedFile.close();

    std::vector<unsigned char> decryptedKey(binCipher.size() - crypto_box_SEALBYTES);
    
    if (crypto_box_seal_open(decryptedKey.data(), 
                             reinterpret_cast<const unsigned char*>(binCipher.data()), 
                             binCipher.size(), 
                             pubKey.data(), 
                             reinterpret_cast<const unsigned char*>(privKeyData.data())) != 0) {
        qCritical() << "RSA Decryption Failed! Wrong Private Key or corrupted file.";
        return {};
    }

    return decryptedKey; 
}