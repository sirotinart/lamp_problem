#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QErrorMessage>
#include <QTextEdit>

#include <socketlistener.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace Ivideon {

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

public slots:
  void enableLamp(const QByteArray& = QByteArray());
  void disableLamp(const QByteArray& = QByteArray());
  void changeLampColor(const QByteArray &data);
  void logMessage(const QString message);

private:
  void loadImage(const QString filename);
  void setLampColor(QGraphicsEllipseItem *item, const QColor color);

private slots:
  void handleCommand(Command command);
  void connectButtonClicked();
  void disconnectButtonClicked();

  void connected();
  void disconnected();
  void errorOccurred(const QString error);

private:
  Ui::MainWindow *ui_;

  QPushButton *connectButton_;
  QLineEdit *addressField_;
  QLineEdit *portField_;
  QGraphicsView *graphicsView_;
  QGraphicsScene graphicsScene_;
  QGraphicsEllipseItem *lampItem_;
  QTextEdit *logField_;

  SocketListener listener_;

  QErrorMessage errorDialog_;
};

}

#endif // MAINWINDOW_H
