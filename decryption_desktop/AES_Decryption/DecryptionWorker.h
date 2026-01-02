/*
 * Copyright (c) 2026 Aditya [Your Last Name]
 * This file is part of Cpp_Encryption_Suite.
 * * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#ifndef DECRYPTIONWORKER_H
#define DECRYPTIONWORKER_H

#include <QObject>
#include <QString>
#include <vector>

class DecryptionWorker : public QObject
{
    Q_OBJECT
public:
    explicit DecryptionWorker(QString encFolder, QString outFolder, std::vector<unsigned char> key, QObject *parent = nullptr);

public slots:
    void process();

signals:
    void progressChanged(int percent);
    void fileFinished(QString fileName);
    void finished();
    void errorOccurred(QString msg);

private:
    QString m_encFolder;
    QString m_outFolder;
    std::vector<unsigned char> m_key;
};

#endif