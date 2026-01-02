/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "AES_Key_genration.h"
#include <stdexcept>

std::vector<unsigned char> AESKeyGeneration::generate256BitKey() {
    if (sodium_init() < 0) {
        throw std::runtime_error("Sodium library failed to initialize");
    }
    std::vector<unsigned char> key(crypto_secretbox_KEYBYTES); // 32 bytes = 256 bits
    crypto_secretbox_keygen(key.data());
    return key;
}