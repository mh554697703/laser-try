#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QByteArray>
#include "lasertrysetting.h"
#include "settingfile.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void connectLaser();
    void beginLaser();
    void setPower(int &s);
    void closeLaser();
    void connectLaser2();
    void beginLaser2();
    void setPower2(int &s);
    void closeLaser2();
private slots:
    void Read_Data2();

    void checkLaser();

    void Read_Data11();

    void checkLaser2();

    void on_openButton_clicked();

    void on_closeButton_clicked();

    void on_freshButton_clicked();

    void on_powerButton_clicked();

    void on_connectButton_clicked();

    void on_disconnectButton_clicked();

    void on_openButton_2_clicked();

    void on_closeButton_2_clicked();

    void on_freshButton_2_clicked();

    void on_connectButton_2_clicked();

    void on_disconnectButton_2_clicked();

    void on_powerButton_2_clicked();

//    void on_saveButton_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial,*serial2;
    QTimer *time_clock,*timer,*time_clock2,*timer2;
    QByteArray myData,senddata,myData2,senddata2;
    QString laserPort,laserPort2;
    char ConvertHexChar(char ch);
    void StringToHex(QString str, QByteArray &senddata);
    void Read_Data();
    void Read_Data1();
    bool powerSet,fire,powerSet2,fire2,setRight,setRight2;
    QString path;
    LASERTRYSETTING mysetting;
    settingfile mySettingFile;

};

#endif // MAINWINDOW_H
