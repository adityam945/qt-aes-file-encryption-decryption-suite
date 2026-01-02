/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef AES_FILE_DECRYPTION_H
#define AES_FILE_DECRYPTION_H

#include <QString>
#include <vector>
#include <functional>

class AESFileDecryption {
public:
// static bool decryptFile(const QString &encFilePath, const std::vector<unsigned char> &rawKey, const QString &outputRootDir);
    static bool decryptFile(const QString &encFilePath, const std::vector<unsigned char> &rawKey, const QString &outputRootDir,
                            std::function<void(int)> progressCallback = nullptr);
};

#endif