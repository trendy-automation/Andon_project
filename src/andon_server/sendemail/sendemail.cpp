#include "sendemail.h"

#include <iostream>
#include <QDebug>

//using namespace std;

SendEmail::SendEmail(QObject *parent) :
    QObject(parent)
{
    sender = stringToEmail(EMAIL_ADDRESS);
    smtp.setAuthMethod(EMAIL_AUTH ? SmtpClient::AuthLogin : SmtpClient::AuthPlain);
    smtp.setHost(EMAIL_HOST);
    smtp.setPort(EMAIL_PORT);
    smtp.setUser(EMAIL_USER);
    smtp.setPassword(EMAIL_PASS);
    smtp.setConnectionType(EMAIL_SSL ? SmtpClient::SslConnection : SmtpClient::TcpConnection);
}

SendEmail::~SendEmail()
{
}

EmailAddress* SendEmail::stringToEmail(const QString &str)
{
    int p1 = str.indexOf("<");
    int p2 = str.indexOf(">");

    if (p1 == -1)
    {
        // no name, only email address
        return new EmailAddress(str);
    }
    else
    {
        return new EmailAddress(str.mid(p1 + 1, p2 - p1 - 1), str.left(p1));
    }

}

void SendEmail::sendEmail(const QString &subject, const QString &message,
                          const QStringList &rcptStringList, QList<QBuffer*> * attachments)
{
    EmailMessage=new MimeMessage;
    EmailMessage->setSender(sender);
    EmailMessage->setSubject(subject);
    for (int i = 0; i < rcptStringList.size(); ++i)
         EmailMessage->addRecipient(stringToEmail(rcptStringList.at(i)));
    MimeHtml content;
    content.setHtml(message);
    EmailMessage->addPart(&content);
    if (attachments)
        for (auto buf = attachments->begin(); buf != attachments->end(); ++buf)
            EmailMessage->addPart(new MimeAttachment(buf.operator *()->data(),
                                                    buf.operator *()->property("FILE_NAME").toString()));
    if (!smtp.connectToHost()) {
        emit errorMessage(QString("Connection Failed to host %1:%2").arg(smtp.getHost()).arg(smtp.getPort()));
        return;
    }
    if(EMAIL_AUTH)
        if (!smtp.login()) {
            emit errorMessage(QString("Authentification Failed by user %1 pass %2").arg(smtp.getUser()).arg(smtp.getPassword()));
            return;
        }
    if (!smtp.sendMail(*EmailMessage)) {
        emit errorMessage("Mail sending failed");
        return;
    } else {
        qDebug()<<"SendEmail::sendEmail"<<"The email was succesfully sent.";
    }
    smtp.quit();
}

