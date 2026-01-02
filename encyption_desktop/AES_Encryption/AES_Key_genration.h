/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef AES_KEY_GENRATION_H
#define AES_KEY_GENRATION_H

#include <sodium.h>
#include <vector>

class AESKeyGeneration {
public:
    static std::vector<unsigned char> generate256BitKey();
};

#endif