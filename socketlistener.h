#ifndef SOCKETLISTENER_H
#define SOCKETLISTENER_H

#include <QObject>
#include <QtNetwork>

#include <types.h>

namespace Ivideon {

class SocketListener : public QObject
{
  Q_OBJECT
public:
  explicit SocketListener(QObject *parent = nullptr);
  void connectToServer(const QString &address, quint16 port);
  void disconnectFromServer();

private slots:
  void readData();
  void connected();
  void disconnected();
  void errorHandler(QAbstractSocket::SocketError error);

signals:
  void commandArrived(Command command);
  void connectedToServer();
  void disconnectedFromServer();
  void logMessage(const QString message);
  void errorOccurred(const QString error);

private:
  QTcpSocket socket_;
  Command command_;
};

}

#endif // SOCKETLISTENER_H
