/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QDebug>
#include <vector>
#include "AES_File_Decryption.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 1. Set your paths
    // Point this to the specific timestamped folder created during encryption
    QString encryptionFolder = "/home/aditya/all_my_work/sql/encryption_20251228_213602";
    QString outputRootDir    = "/home/aditya/all_my_work/sql/decrypted_output";

    QDir dir(encryptionFolder);
    if (!dir.exists()) {
        qCritical() << "Target encryption folder does not exist:" << encryptionFolder;
        return -1;
    }

    // 2. Locate and Load the Key File
    QStringList keyFilters;
    keyFilters << "key_*.txt";
    QFileInfoList keyList = dir.entryInfoList(keyFilters, QDir::Files);

    if (keyList.isEmpty()) {
        qCritical() << "No key file found in" << encryptionFolder;
        return -1;
    }

    // Load the first key found
    QFile keyFile(keyList.first().absoluteFilePath());
    keyFile.open(QIODevice::ReadOnly);
    QByteArray hexKey = keyFile.readAll().trimmed();
    QByteArray binKey = QByteArray::fromHex(hexKey);
    keyFile.close();

    std::vector<unsigned char> sharedKey(binKey.begin(), binKey.end());
    qDebug() << "Key loaded successfully. Starting batch decryption...";

    // 3. Iterate through all files in the folder (except the key itself)
    QDirIterator it(encryptionFolder, QDir::Files);
    int successCount = 0;
    int failCount = 0;

    while (it.hasNext()) {
        QString encFilePath = it.next();
        QFileInfo encInfo(encFilePath);

        // Skip the key file
        if (encInfo.fileName().startsWith("key_")) continue;

        if (AESFileDecryption::decryptFile(encFilePath, sharedKey, outputRootDir)) {
            successCount++;
        } else {
            failCount++;
            qWarning() << "Failed to decrypt:" << encInfo.fileName();
        }
    }

    qDebug() << "------------------------------------------";
    qDebug() << "Decryption Task Finished";
    qDebug() << "Successfully restored:" << successCount << "files";
    if (failCount > 0) qDebug() << "Failed:" << failCount << "files";
    qDebug() << "Output location:" << outputRootDir;

    return 0;
}