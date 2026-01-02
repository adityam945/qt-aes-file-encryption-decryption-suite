/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef AES_FILE_ENCRYPTION_H
#define AES_FILE_ENCRYPTION_H

#include <QString>
#include <vector>
#include<string>
#include <functional>

#include "EncryptionWorker.h"

class AESFileEncryption {
public:
    // static bool encryptFile(const QString &filePath, const QString &baseInputDir, const QString &targetEncFolder, const std::vector<unsigned char> &key);

    static bool encryptFile(const QString &filePath, const QString &baseInputDir, const QString &targetEncFolder, const std::vector<unsigned char> &key, 
            std::function<void(int)> progressCallback = nullptr // parts
        );
                    
};

#endif
