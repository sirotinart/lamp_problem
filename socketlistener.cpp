#include "socketlistener.h"

#ifdef linux
#include <arpa/inet.h>
#else
#include <Winsock.h>
#endif

namespace Ivideon {

SocketListener::SocketListener(QObject *parent)
  : QObject(parent)
{
  connect(&socket_, &QTcpSocket::connected, this, &SocketListener::connected);
  connect(&socket_, &QTcpSocket::disconnected, this, &SocketListener::disconnected);
  connect(&socket_, &QTcpSocket::errorOccurred, this, &SocketListener::errorHandler);
}

void SocketListener::connectToServer(const QString &address, quint16 port)
{
  emit logMessage(QString("Connecting to server, address: %1, port %2").arg(address).arg(port));

  QUrl serverAddress(address);

  if (!serverAddress.isValid()){
    emit logMessage("Failed to connect to server: invalid address");
    return;
  }

  socket_.connectToHost(serverAddress.toString(), port);
}

void SocketListener::disconnectFromServer()
{
  socket_.disconnectFromHost();
}

void SocketListener::readData()
{
  static bool readingHeader = true;

  if (readingHeader) {
    if (socket_.bytesAvailable() < static_cast<qint64>(sizeof(CommandHeader))) {
      return;
    }

    CommandHeader header;

    qint64 ret = socket_.read(reinterpret_cast<char *>(&header), sizeof(header));

    if (ret != sizeof(header)) {
      emit logMessage("Failed to read packet header");
      return;
    }

    command_.header.type = header.type;
    command_.header.length = ntohs(header.length);

    if (command_.header.length == 0) {
      emit commandArrived(command_);
      return;
    }

    readingHeader = false;
  }

  if (!readingHeader) {
    if (socket_.bytesAvailable() < static_cast<qint64>(command_.header.length)) {
      return;
    }

    command_.data = socket_.read(command_.header.length);

    if (command_.data.size() != command_.header.length) {
      readingHeader = true;

      emit logMessage("Failed to read packet data");
      return;
    }

    emit commandArrived(command_);

    readingHeader = true;
  }
}

void SocketListener::connected()
{
  connect(&socket_, &QTcpSocket::readyRead, this, &SocketListener::readData);

  emit connectedToServer();
}

void SocketListener::disconnected()
{
  disconnect(&socket_, &QTcpSocket::readyRead, this, &SocketListener::readData);

  emit disconnectedFromServer();
}

void SocketListener::errorHandler(QAbstractSocket::SocketError error)
{
  disconnect(&socket_, &QTcpSocket::readyRead, this, &SocketListener::readData);

  emit errorOccurred(QString("Connection error: %1 (%2)").arg(error).arg(socket_.errorString()));
}

}
