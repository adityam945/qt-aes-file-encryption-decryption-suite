/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "EncryptionWorker.h"
#include "AES_File_encryption.h"
#include <QDirIterator>
#include <QDebug>

EncryptionWorker::EncryptionWorker(QString sourceDir, QString outputDir, std::vector<unsigned char> key, QObject *parent)
    : QObject(parent), m_sourceDir(sourceDir), m_outputDir(outputDir), m_key(key) {}

void EncryptionWorker::process() {
    QDirIterator it(m_sourceDir, QDir::Files, QDirIterator::Subdirectories);
    
    // Count total files for overall progress
    int processedCount = 0;
    
    while (it.hasNext()) {
        QString filePath = it.next();
        if (filePath.contains("/encryption_") || filePath.contains("/db/")) continue;

        emit fileFinished("Encrypting: " + it.fileName());

        bool success = AESFileEncryption::encryptFile(filePath, m_sourceDir, m_outputDir, m_key, 
            [this](int percent) {
                emit progressChanged(percent); 
            });

        if (!success) {
            emit errorOccurred("Failed: " + filePath);
        }
        processedCount++;
    }

    emit finished();
}