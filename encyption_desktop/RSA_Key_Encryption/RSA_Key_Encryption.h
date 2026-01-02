/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef RSA_KEY_ENCRYPTION_H
#define RSA_KEY_ENCRYPTION_H

#include <QString>
#include <vector>
#include <QFile>

class RSAKeyEncryption {
public:
    // static QFile wrapKey(const QString &inputKeyPath, const QString &machinePublicKeyHex);
    static bool wrapKey(const QString &inputKeyPath, const QString &machinePublicKeyHex, QString &outputKeyPath);
};

#endif