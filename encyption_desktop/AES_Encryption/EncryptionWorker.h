/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef ENCRYPTIONWORKER_H
#define ENCRYPTIONWORKER_H

#include <QObject>
#include <QString>
#include <vector>

class EncryptionWorker : public QObject
{
    Q_OBJECT
public:
    explicit EncryptionWorker(QString sourceDir, QString outputDir, std::vector<unsigned char> key, QObject *parent = nullptr);

public slots:
    void process();
    // void process_thread()
    // void spawn_process_threads()

signals:
    void progressChanged(int percent);
    void fileFinished(QString fileName);

    void finished();
    void errorOccurred(QString msg);

private:
    QString m_sourceDir;
    QString m_outputDir;
    std::vector<unsigned char> m_key;
};

#endif