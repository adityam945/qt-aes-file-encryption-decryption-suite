/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "DecryptionWorker.h"
#include "AES_File_Decryption.h" // Ensure this header is accessible
#include <QDirIterator>
#include <QDebug>

DecryptionWorker::DecryptionWorker(QString encFolder, QString outFolder, std::vector<unsigned char> key, QObject *parent)
    : QObject(parent), m_encFolder(encFolder), m_outFolder(outFolder), m_key(key) {}

void DecryptionWorker::process() {
    QDirIterator it(m_encFolder, QDir::Files);
    
    while (it.hasNext()) {
        QString filePath = it.next();
        QString fileName = it.fileName();

        // Skip non-encrypted files (keys, etc.)
        if (fileName.endsWith(".rsa_locked") || fileName.endsWith(".txt") || fileName.endsWith(".private_key")) continue;

        // Callback Lambda for Progress
        bool success = AESFileDecryption::decryptFile(filePath, m_key, m_outFolder, 
            [this](int percent) {
                emit progressChanged(percent);
            });

        if (success) {
            emit fileFinished(fileName);
        } else {
            emit errorOccurred("Failed: " + fileName);
        }
    }

    emit finished();
}