#include "MyClient.h"
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QTime>
#include <QByteArray>


MyClient::MyClient(QWidget*       pwgt
                  ) : QWidget(pwgt)
                    , m_nNextBlockSize(0)
{
    m_pTcpSocket = new QSslSocket(this);

    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,         SLOT(slotError(QAbstractSocket::SocketError))
           );

    m_ptxtHost  =  new QLineEdit;
    m_ptxtPort  =  new QLineEdit;
    m_ptxtLogin  = new QLineEdit;
    m_ptxtTo  =    new QLineEdit;
    m_ptxtPass =   new QLineEdit;
    m_ptxtSubj =   new QLineEdit;
    m_ptxtText =   new QPlainTextEdit;

    m_plHost = new QLabel("Host");
    m_plPort = new QLabel("Port");
    m_plLogin= new QLabel("Login");
    m_plPass = new QLabel("Pass");
    m_plTo =   new QLabel("To");
    m_plSubj = new QLabel("Subject");
    m_plText = new QLabel("Massege");

    m_ptxtInfo =  new QTextEdit;

    m_ptxtInfo-> setReadOnly(true);

    m_ptxtPass->setEchoMode(QLineEdit::EchoMode::Password);

    QPushButton* pcmd = new QPushButton("&Send");
    connect(pcmd, SIGNAL(clicked()), SLOT(slotSendToServer()));

    m_ptxtHost-> setText("smtp.gmail.com");
    m_ptxtPort-> setText("465");

    pvbxLayout = new QGridLayout(this);

    pvbxLayout->addWidget(new QLabel("<H1>SMTP-Client</H1>"),0,0);
    pvbxLayout->addWidget(m_plHost,     1, 0);
    pvbxLayout->addWidget(m_ptxtHost,   1, 1);
    pvbxLayout->addWidget(m_plPort,     2, 0);
    pvbxLayout->addWidget(m_ptxtPort,   2, 1);
    pvbxLayout->addWidget(m_plLogin,    3, 0);
    pvbxLayout->addWidget(m_ptxtLogin,  3, 1);
    pvbxLayout->addWidget(m_plPass,     4, 0);
    pvbxLayout->addWidget(m_ptxtPass,   4, 1);
    pvbxLayout->addWidget(m_plTo,       5, 0);
    pvbxLayout->addWidget(m_ptxtTo,     5, 1);
    pvbxLayout->addWidget(m_plSubj,     6, 0);
    pvbxLayout->addWidget(m_ptxtSubj,   6, 1);
    pvbxLayout->addWidget(m_plText,     7, 0);
    pvbxLayout->addWidget(m_ptxtText,   7, 1);
    pvbxLayout->addWidget(pcmd,         8, 1, 1, 1);
    pvbxLayout->addWidget(m_ptxtInfo,   9, 0, 1, 2);

    setLayout(pvbxLayout);
}
void MyClient::slotReadyRead()
{
   QString responseLine;
   do
   {
       responseLine = m_pTcpSocket->readLine();
       response += responseLine;
   }
   while ( m_pTcpSocket->canReadLine() && responseLine[3] != ' ' );

   responseLine.truncate( 3 );

   qDebug() << "Server response: " << response;

   QTextStream *out = new QTextStream(m_pTcpSocket);

   if (state == HandShake && responseLine == "220"){
       qDebug()<<"EHLO localhost";
       *out <<"EHLO localhost"<<"\r\n";
       out->flush();
       state = Auth;
   }
   else if(state == Auth && responseLine == "250"){
       qDebug()<<"AUTH LOGIN";
       *out <<"AUTH LOGIN"<<"\r\n";
       out->flush();
       state = User;
   }
   else if(state == User && responseLine == "334"){
       qDebug()<<"User";
       *out <<QByteArray().append(m_ptxtLogin->text()).toBase64()<<"\r\n";
       out->flush();
       state = Pass;
   }
   else if(state == Pass && responseLine == "334"){
       qDebug()<<"Pass";
       *out <<QByteArray().append(m_ptxtPass->text()).toBase64()<<"\r\n";
       out->flush();
       state = Mail;
   }
   else if(state == Mail && responseLine == "235"){
       qDebug()<<"MAIL FROM";
       *out <<"MAIL FROM: <"<<m_ptxtLogin->text()<<">\r\n";
       out->flush();
       state = Rcpt;
   }
   else if(state == Rcpt && responseLine == "250"){
       qDebug()<<"rcpt to:";
       *out <<"rcpt to: <"<<m_ptxtTo->text()<<">\r\n";
       out->flush();
       state = Data;
   }
   else if(state == Data && responseLine == "250"){
       qDebug()<<"DATA";
       *out <<"DATA"<<"\r\n";
       out->flush();
       state = Body;
   }
   else if(state == Body && responseLine == "354"){
       qDebug()<<"Content";
       *out<<"Subject: "<<m_ptxtSubj->text()<<"\r\n";
       *out <<m_ptxtText->toPlainText()<<"\r\n.\r\n";
       out->flush();
       state = Quit;
   }
   else if(state == Quit && responseLine == "250"){
       qDebug()<<"QUIT";
       *out<<"QUIT "<<"\r\n";
       out->flush();
       m_ptxtInfo->append("Message sent");
       state = Close;
   }
   else if (state == Close){
       qDebug()<<"Close";
       m_ptxtLogin ->setText("");
       m_ptxtTo    ->setText("");
       m_ptxtPass  ->setText("");
       m_ptxtSubj  ->setText("");
       m_ptxtText  ->setPlainText("");
       return;
   }
   else{
       state = Close;
       m_ptxtInfo->append("Failed to send message");
   }
   response = "";
}
void MyClient::slotError(QAbstractSocket::SocketError err)
{
    QString strError =
        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                     "The host was not found." :
                     err == QAbstractSocket::RemoteHostClosedError ?
                     "The remote host is closed." :
                     err == QAbstractSocket::ConnectionRefusedError ?
                     "The connection was refused." :
                     QString(m_pTcpSocket->errorString())
                    );
    m_ptxtInfo->append(strError);
}

void MyClient::slotSendToServer()
{
    newMessage();
}

void MyClient::slotConnected()
{
    m_ptxtInfo->append("Received the connected() signal");
}

void MyClient:: newMessage(){
    m_pTcpSocket->abort();
    strHost = m_ptxtHost->text();
    nPort   = m_ptxtPort->text().toInt();
    state = HandShake;
    m_pTcpSocket->connectToHostEncrypted(strHost, nPort);
}
