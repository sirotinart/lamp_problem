#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QValidator>
#include <QMap>
#include <QImageReader>

namespace Ivideon {

static const
QMap<uint32_t, std::function<void (MainWindow*, const QByteArray&)>> handlers {
  {ON, &MainWindow::enableLamp},
  {OFF, &MainWindow::disableLamp},
  {COLOR, &MainWindow::changeLampColor}
};

static const QRect lampPosition(48, 30, 25, 25);
static const QColor lampColor(252, 238, 167, 170);

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui_(new Ui::MainWindow)
  , lampItem_(new QGraphicsEllipseItem(lampPosition))
{
  ui_->setupUi(this);

  connectButton_ = ui_->centralwidget->findChild<QPushButton *>("connectButton");

  addressField_ = ui_->centralwidget->findChild<QLineEdit *>("addresField");
  portField_ = ui_->centralwidget->findChild<QLineEdit *>("portField");
  graphicsView_ = ui_->centralwidget->findChild<QGraphicsView *>("graphicsView");
  logField_ = ui_->centralwidget->findChild<QTextEdit *>("logField");

  portField_->setValidator(new QIntValidator(1, 65535, portField_));

  addressField_->setText("emb2.extcam.xyz");
  portField_->setText("9991");

  graphicsView_->setScene(&graphicsScene_);

  connect(&listener_, &SocketListener::connectedToServer, this, &MainWindow::connected);
  connect(&listener_, &SocketListener::disconnectedFromServer, this, &MainWindow::disconnected);
  connect(&listener_, &SocketListener::logMessage, this, &MainWindow::logMessage);
  connect(&listener_, &SocketListener::errorOccurred, this, &MainWindow::errorOccurred);

  connect(&listener_, &SocketListener::commandArrived, this, &MainWindow::handleCommand);

  connect(connectButton_, &QPushButton::clicked, this, &MainWindow::connectButtonClicked);

  loadImage("lamp.png");

  setLampColor(lampItem_, lampColor);

  lampItem_->setVisible(false);
  graphicsScene_.addItem(lampItem_);
}

MainWindow::~MainWindow()
{
  delete ui_;
}

void MainWindow::enableLamp(const QByteArray &)
{
  if (lampItem_->isVisible()) {
    return;
  }

  setLampColor(lampItem_, lampColor);

  lampItem_->setVisible(true);
}

void MainWindow::disableLamp(const QByteArray &)
{
  if (!lampItem_->isVisible()) {
    return;
  }

  lampItem_->setVisible(false);

  setLampColor(lampItem_, lampColor);
}

void MainWindow::changeLampColor(const QByteArray &data)
{
  if (!lampItem_->isVisible()) {
    qDebug()<<"Failed to set lamp color: lamp is disabled";
    return;
  }

  if (data.size() < 3) {
    qDebug()<<"Failed to set lamp color: invalid data size:"<<data.size();
  }

  int r = static_cast<uint8_t>(data.at(0));
  int g = static_cast<uint8_t>(data.at(1));
  int b = static_cast<uint8_t>(data.at(2));

  setLampColor(lampItem_, QColor(r, g, b, 170));
}

void MainWindow::logMessage(const QString message)
{
  logField_->append(message);
}

void MainWindow::loadImage(const QString filename)
{
  QImageReader reader(filename);
  QImage image = reader.read();

  image = image.scaledToHeight(250);

  if (image.isNull()) {
    logMessage(QString("Failed to read image: %1").arg(reader.errorString()));
    return;
  }

  graphicsScene_.addPixmap(QPixmap::fromImage(image));
}

void MainWindow::setLampColor(QGraphicsEllipseItem *item, const QColor color)
{
  item->setBrush(QBrush(color));
  item->setPen(QPen(QColor(0, 0, 0, 50)));
}

void MainWindow::handleCommand(Command command)
{
  logMessage(QString("Command type: 0x%1 length: %2").arg(command.header.type, 0, 16)
                                                     .arg(command.header.length));

  auto it = handlers.find(command.header.type);

  if (it == handlers.end()) {
    logMessage("Failed to hadle command: invalid command type");
    return;
  }

  auto handler = it.value();

  handler(this, command.data);
}

void MainWindow::connectButtonClicked()
{
  connectButton_->setEnabled(false);
  listener_.connectToServer(addressField_->text(), portField_->text().toInt());
}

void MainWindow::disconnectButtonClicked()
{
  connectButton_->setEnabled(false);
  listener_.disconnectFromServer();
}

void MainWindow::connected()
{
  connectButton_->disconnect();
  connect(connectButton_, &QPushButton::clicked, this, &MainWindow::disconnectButtonClicked);

  connectButton_->setText("Disconnect");
  connectButton_->setEnabled(true);

  logMessage("Connected to server");
}

void MainWindow::disconnected()
{
  connectButton_->disconnect();
  connect(connectButton_, &QPushButton::clicked, this, &MainWindow::connectButtonClicked);

  connectButton_->setText("Connect");
  connectButton_->setEnabled(true);

  logMessage("Disconnected from server");
}

void MainWindow::errorOccurred(const QString error)
{
  connectButton_->disconnect();
  connect(connectButton_, &QPushButton::clicked, this, &MainWindow::connectButtonClicked);

  connectButton_->setText("Connect");
  connectButton_->setEnabled(true);

  logMessage(error);
}

}


