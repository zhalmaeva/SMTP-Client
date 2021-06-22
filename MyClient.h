#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QWidget>
#include <QSslSocket>
#include <QLabel>
#include <QGridLayout>
#include <QPlainTextEdit>

class QTextEdit;
class QLineEdit;

class MyClient : public QWidget{
Q_OBJECT
private:
    QSslSocket*     m_pTcpSocket;
    quint16         m_nNextBlockSize;
    QString         strHost;
    int             nPort;
    int state;

    QGridLayout*    pvbxLayout;

    QLineEdit*      m_ptxtHost;
    QLineEdit*      m_ptxtPort;
    QLineEdit*      m_ptxtLogin;
    QLineEdit*      m_ptxtPass;
    QLineEdit*      m_ptxtTo;
    QLineEdit*      m_ptxtSubj;
    QPlainTextEdit* m_ptxtText;

    QTextEdit*      m_ptxtInfo;

    QLabel*         m_plHost;
    QLabel*         m_plPort;
    QLabel*         m_plLogin;
    QLabel*         m_plPass;
    QLabel*         m_plTo;
    QLabel*         m_plSubj;
    QLabel*         m_plText;
    QString         response;
    enum states     {Tls, HandShake ,Auth,User,Pass,Rcpt,Mail,Data,Init,Body,Quit,Close};


public:
    MyClient(QWidget *pwgt = 0);
    void newMessage();


private slots:
    void slotReadyRead   (                            );
    void slotError       (QAbstractSocket::SocketError);
    void slotSendToServer(                            );
    void slotConnected   (                            );
};

#endif // MYCLIENT_H
