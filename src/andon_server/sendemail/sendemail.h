/*
  Copyright (c) 2011 - Tőkés Attila

  This file is part of SmtpClient for Qt.

  SmtpClient for Qt is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  SmtpClient for Qt is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY.

  See the LICENCE file for more details.
*/

#ifndef SENDEMAIL_H
#define SENDEMAIL_H

#include <QObject>

#include "../../lib/SmtpClient/src/SmtpMime"
//___________
//#include "SmtpMime"
//___________
////??????????
//#include "mimepart.h"
//#include "mimehtml.h"
//#include "mimeattachment.h"
//#include "mimemessage.h"
//#include "mimetext.h"
//#include "mimeinlinefile.h"
//#include "mimefile.h"
////??????????

#include <QBuffer>

class SendEmail : public QObject
{
    Q_OBJECT

public:
    explicit SendEmail(QObject *parent = 0);
    ~SendEmail();
    static EmailAddress * stringToEmail(const QString & str);

public slots:
//    void sendEmail(const QString &subject, const QString &message, const QStringList &rcptStringList);
    bool sendEmail(const QString &subject, const QString &message, const QStringList &rcptStringList,
                   const QList<QBuffer*> &attachments=QList<QBuffer*>());
//    void addAttachment(const QByteArray &stream, const QString &fileName);
//    void sendAttachment(const QString &subject, const QString &message,
//                        const QStringList &rcptStringList, QList<QBuffer*> *attachments);

signals:
    void errorMessage(const QString & message);

private:
 EmailAddress   *sender;
 SmtpClient     smtp;
 MimeMessage    *EmailMessage;

};

#endif // SENDEMAIL_H
