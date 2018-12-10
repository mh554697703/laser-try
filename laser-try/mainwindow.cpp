#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include<QDebug>
#include<QSerialPort>
#include<QListView>
#include<QtMath>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QDir dir;
    path = dir.currentPath()+"/"+"LTsettingFile.ini";
    mySettingFile.CreatSetFile(path);
    mySettingFile.ReadSetFile(path);
    mysetting = mySettingFile.GetSetting();

    serial = new QSerialPort;
    serial2 = new QSerialPort;
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::Read_Data);  //有可读信号时开始读取
    connect(serial2, &QSerialPort::readyRead, this, &MainWindow::Read_Data1);  //有可读信号时开始读取
    time_clock = new QTimer(this); //建立串口读写超时定时器
    timer = new QTimer(this);
    time_clock2 = new QTimer(this); //建立串口读写超时定时器
    timer2 = new QTimer(this);
    connect(time_clock,SIGNAL(timeout()),this,SLOT(Read_Data2()));
    connect(timer,SIGNAL(timeout()),this,SLOT(checkLaser()));
    connect(time_clock2,SIGNAL(timeout()),this,SLOT(Read_Data11()));
    connect(timer2,SIGNAL(timeout()),this,SLOT(checkLaser2()));
//    laserPort="COM8";
    powerSet=true;
    powerSet2=true;
    fire=false;
    fire2=false;
    setRight=false;
    setRight2=false;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
       {
           QSerialPort seriala;
           seriala.setPort(info);
           if(seriala.open(QIODevice::ReadWrite))
           {
               ui->com->addItem(seriala.portName());
               ui->com_2->addItem(seriala.portName());
               seriala.close();
           }
       }
    ui->progressBar_seedSet->setValue(0);
    ui->progressBar_seedOpen->setValue(0);
    ui->progressBar_seedSet_2->setValue(0);
    ui->progressBar_seedOpen_2->setValue(0);

    ui->com->setView(new QListView());
    ui->com_2->setView(new QListView());

    ui->com->setCurrentText(mysetting.com1);             //自动设置初始值
    ui->com_2->setCurrentText(mysetting.com2);
    ui->power->setValue(mysetting.seedPower);
    ui->power_2->setValue(mysetting.pulsePower);
}

MainWindow::~MainWindow()
{
    mysetting.com1 = ui->com->currentText();
    mysetting.com2 = ui->com_2->currentText();
    mysetting.seedPower = ui->power->text().toInt();
    mysetting.pulsePower = ui->power_2->text().toInt();
    mySettingFile.WriteSetFile(mysetting,path);
    delete ui;
}

void MainWindow::connectLaser()
{
    serial->setPortName(laserPort);
    if(!serial->open(QIODevice::ReadWrite))
    {qDebug()<<"open error";
        return;}

    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setParity(QSerialPort::NoParity);
}

void MainWindow::beginLaser()
{
//   connectLaser();
   StringToHex("AA 55 C1 01 01 01 00",senddata);
   serial->write(senddata);
}

void MainWindow::setPower(int &s)
{
    QString key =QString("%1").arg(s,4,16,QLatin1Char('0')).toUpper();
    bool ok;
    int aa=key.left(2).toInt(&ok,16)+key.right(2).toInt(&ok,16);
    QString key2 =QString("%1").arg(aa,4,16,QLatin1Char('0')).toUpper();
    QString power="AA 55 C3 02 "+key.right(2)+" "+key.left(2)+" "+key2.right(2)+" "+key2.left(2);
    qDebug()<<"power="<<power;
    StringToHex(power,senddata);
    serial->write(senddata);
    powerSet=false;
}

void MainWindow::closeLaser()
{
    StringToHex("AA 55 C1 01 00 00 00",senddata);
    serial->write(senddata);
}

void MainWindow::connectLaser2()
{
    serial2->setPortName(laserPort2);
    if(!serial2->open(QIODevice::ReadWrite))
    {qDebug()<<"open error";
        return;}

    serial2->setBaudRate(QSerialPort::Baud9600);
    serial2->setDataBits(QSerialPort::Data8);
    serial2->setStopBits(QSerialPort::OneStop);
    serial2->setParity(QSerialPort::NoParity);
}

void MainWindow::beginLaser2()
{
    StringToHex("AA 55 C1 01 01 01 00",senddata);
    serial2->write(senddata);
    fire2=true;
}

void MainWindow::setPower2(int &s)
{
    QString key =QString("%1").arg(s,4,16,QLatin1Char('0')).toUpper();
    bool ok;
    int aa=key.left(2).toInt(&ok,16)+key.right(2).toInt(&ok,16);
    QString key2 =QString("%1").arg(aa,4,16,QLatin1Char('0')).toUpper();
    QString power="AA 55 C3 02 "+key.right(2)+" "+key.left(2)+" "+key2.right(2)+" "+key2.left(2);
    qDebug()<<"power="<<power;
    StringToHex(power,senddata);
    serial2->write(senddata);
    powerSet2=false;
}

void MainWindow::closeLaser2()
{
    senddata = QByteArray::fromHex("AA 55 C1 01 00 00 00");
    //StringToHex("AA 55 C1 01 00 00 00",senddata);
    serial2->write(senddata);
}
void MainWindow::StringToHex(QString str, QByteArray &senddata)
{
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        //char lstr,
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}

char MainWindow::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}
void MainWindow::Read_Data()
{
    if (!time_clock->isActive())
    {time_clock->start(300);}
    myData.append(serial->readAll());//读串口缓冲区QByteArray数据 关键是append
}

void MainWindow::Read_Data1()
{
    if (!time_clock2->isActive())
    {time_clock2->start(300);}
    myData2.append(serial2->readAll());//读串口缓冲区QByteArray数据 关键是append
}
void MainWindow::Read_Data2()
{
    time_clock->stop();
    QString temp= myData.toHex();
    qDebug()<<"back="<<temp;
    myData.clear();
    if(!powerSet)
    {
        QString powerAnswer=temp.mid(8,2);
        if(powerAnswer!="00")
        {
            qDebug()<<QString::fromLocal8Bit("功率设置错误");
            ui->state->setText(QString::fromLocal8Bit("功率设置错误"));
            ui->progressBar_seedSet->setValue(0);
            setRight=false;
        }
        else
        {
            ui->state->setText(QString::fromLocal8Bit("功率设置成功"));
            timer->start(1000);
            powerSet=true;
            ui->progressBar_seedSet->setValue(100);
            setRight=true;
        }
    }
    else
    {
        if(fire)
        {
            if(temp=="55aac101000000")
            {
                ui->state->setText(QString::fromLocal8Bit("激光器打开正常"));
                ui->progressBar_seedOpen->setValue(100);
                fire=false;
            }

        }
        else
        {
            qDebug()<<"temp=";
            QString checkAnswer=temp.mid(8,2);
            if(checkAnswer=="00")
            {
                qDebug()<<QString::fromLocal8Bit("电源关闭");
                ui->state->setText(QString::fromLocal8Bit("电源关闭"));
                ui->progressBar_seedSet->setValue(0);
                ui->progressBar_seedOpen->setValue(0);
                setRight=false;
            }
            else
            {
                QString checkAnswer=temp.mid(10,2);
                QString checkOpen=temp.mid(8,2);
                if(checkOpen=="01")
                {
                    ui->progressBar_seedOpen->setValue(100);
                    if(setRight)
                    {
                      ui->progressBar_seedSet->setValue(100);
                    }
                    else
                    {ui->progressBar_seedSet->setValue(0);}
                    if(checkAnswer!="00")
                    {
                        qDebug()<<QString::fromLocal8Bit("激光器异常");
                        ui->state->setText(QString::fromLocal8Bit("激光器异常!!!!"));
                        ui->progressBar_seedSet->setValue(0);
                        bool ok;
                        int s=checkAnswer.right(1).toInt(&ok,16);
                        QString key =QString("%1").arg(s,4,2,QLatin1Char('0'));
                        if(key.left(1)=="1")
                        {qDebug()<<QString::fromLocal8Bit("泵浦温度异常");}
                        if(key.right(1)=="1")
                        {qDebug()<<QString::fromLocal8Bit("模块温度异常");}
                        if(key.mid(1,1)=="1")
                        {qDebug()<<QString::fromLocal8Bit("输入功率异常");}
                        if(key.mid(2,1)=="1")
                        {qDebug()<<QString::fromLocal8Bit("种子激光器温度异常");}
                    }
                    else
                    {
    //                  qDebug()<<"工作正常";
                      ui->state->setText(QString::fromLocal8Bit("工作正常"));
                    }
                }
                else
                {
                    ui->progressBar_seedOpen->setValue(0);
                    ui->state->setText(QString::fromLocal8Bit("激光器关闭"));
                }
            }
        }
        }


}


void MainWindow::checkLaser()
{
    StringToHex("AA 55 D3 00 00 00",senddata);
    serial->write(senddata);
    //    timer->stop();
}

void MainWindow::Read_Data11()
{
    time_clock2->stop();
    QString temp= myData2.toHex();
    qDebug()<<"back="<<temp;
    myData2.clear();
    if(!powerSet2)
    {
        QString powerAnswer=temp.mid(8,2);
        if(powerAnswer!="00")
        {
            qDebug()<<QString::fromLocal8Bit("功率设置错误");
            ui->state_2->setText(QString::fromLocal8Bit("功率设置错误"));
            ui->progressBar_seedSet_2->setValue(0);
            setRight2=false;
        }
        else
        {
            ui->state_2->setText(QString::fromLocal8Bit("功率设置成功"));
            timer2->start(1000);
            powerSet2=true;
            ui->progressBar_seedSet_2->setValue(100);
            setRight2=true;
        }
    }
    else
    {
        if(fire2)
        {
            if(temp=="55aac101000000")
            {
                ui->state_2->setText(QString::fromLocal8Bit("激光器打开正常"));
                ui->progressBar_seedOpen_2->setValue(100);
                ui->disconnectButton->setEnabled(false);
                fire2=false;
            }            
        }
        else
        {
            qDebug()<<"temp=";
            QString checkAnswer=temp.mid(8,2);
            if(checkAnswer=="00")
            {
                qDebug()<<QString::fromLocal8Bit("电源关闭");
                ui->state_2->setText(QString::fromLocal8Bit("电源关闭"));
                ui->progressBar_seedSet_2->setValue(0);
                ui->progressBar_seedOpen_2->setValue(0);
                ui->disconnectButton->setEnabled(true);
                setRight2=false;
            }
            else
            {
                QString checkAnswer=temp.mid(10,2);
                QString checkOpen=temp.mid(8,2);
                if(checkOpen=="01")
                {
                    ui->progressBar_seedOpen_2->setValue(100);
                    ui->disconnectButton->setEnabled(false);
                    if(setRight2)
                    {
                      ui->progressBar_seedSet_2->setValue(100);
                    }
                    else
                    {ui->progressBar_seedSet_2->setValue(0);}
                    if(checkAnswer!="00")
                    {
                        qDebug()<<QString::fromLocal8Bit("激光器异常");
                        ui->state_2->setText("激光器异常");
                        ui->progressBar_seedSet_2->setValue(0);
                        bool ok;
                        int s=checkAnswer.right(1).toInt(&ok,16);
                        QString key =QString("%1").arg(s,4,2,QLatin1Char('0'));
                        if(key.left(1)=="1")
                        {qDebug()<<QString::fromLocal8Bit("泵浦温度异常");}
                        if(key.right(1)=="1")
                        {qDebug()<<QString::fromLocal8Bit("模块温度异常");}
                        if(key.mid(1,1)=="1")
                        {qDebug()<<QString::fromLocal8Bit("输入功率异常");}
                        if(key.mid(2,1)=="1")
                        {qDebug()<<QString::fromLocal8Bit("种子激光器温度异常");}
                    }
                    else
                    {
    //                  qDebug()<<"工作正常";
                      ui->state_2->setText(QString::fromLocal8Bit("工作正常"));
                      ui->disconnectButton->setEnabled(false);
                    }
                }
                else
                {
                    ui->progressBar_seedOpen_2->setValue(0);
                    ui->disconnectButton->setEnabled(true);
                    ui->state_2->setText(QString::fromLocal8Bit("激光器关闭"));
                }
            }
        }
        }
}

void MainWindow::checkLaser2()
{
    StringToHex("AA 55 D3 00 00 00",senddata);
    serial2->write(senddata);
}

void MainWindow::on_openButton_clicked()
{
    laserPort=ui->com->currentText();
    serial->setPortName(laserPort);
    if(!serial->open(QIODevice::ReadWrite))
    {qDebug()<<"open error";
        return;}

    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setParity(QSerialPort::NoParity);
    ui->state->setText(QString::fromLocal8Bit("串口连接成功"));
    ui->openButton->setEnabled(false);
    ui->freshButton->setEnabled(false);
     ui->closeButton->setEnabled(true);

     timer->start(1000);
}

void MainWindow::on_closeButton_clicked()
{
    if(timer->isActive())
    {timer->stop();}
    serial->close();
    ui->state->setText(QString::fromLocal8Bit("串口断开"));
    ui->closeButton->setEnabled(false);
    ui->openButton->setEnabled(true);
    ui->freshButton->setEnabled(true);
}

void MainWindow::on_freshButton_clicked()
{
    double a = qRadiansToDegrees(atan2(1,1));
    double b = qRadiansToDegrees(atan2(-1,-1));
    qDebug()<<"a="<<a;
    qDebug()<<"b="<<b;
    ui->com->clear();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort seriala;
        seriala.setPort(info);
        if(seriala.open(QIODevice::ReadWrite))
        {
            ui->com->addItem(seriala.portName());
            seriala.close();
        }
    }
    ui->closeButton->setEnabled(true);
    ui->openButton->setEnabled(true);
}

void MainWindow::on_powerButton_clicked()
{
    powerSet=false;
    QString toSend=ui->power->text();
//    int i=(toSend.split("uw").at(0)).toInt();
    int i=toSend.toInt();
    qDebug()<<i;
    setPower(i);
}

void MainWindow::on_connectButton_clicked()
{
    if(timer->isActive())
    {timer->stop();}
    fire=true;
    beginLaser();
}

void MainWindow::on_disconnectButton_clicked()
{
    timer->stop();
    closeLaser();
}

void MainWindow::on_openButton_2_clicked()
{
    laserPort2=ui->com_2->currentText();
    serial2->setPortName(laserPort2);
    if(!serial2->open(QIODevice::ReadWrite))
    {qDebug()<<"open error";
        return;}

    serial2->setBaudRate(QSerialPort::Baud9600);
    serial2->setDataBits(QSerialPort::Data8);
    serial2->setStopBits(QSerialPort::OneStop);
    serial2->setParity(QSerialPort::NoParity);
    ui->state_2->setText(QString::fromLocal8Bit("串口连接成功"));
    ui->openButton_2->setEnabled(false);
    ui->freshButton_2->setEnabled(false);
    ui->closeButton_2->setEnabled(true);

    timer2->start(1000);
}

void MainWindow::on_closeButton_2_clicked()
{
    if(timer2->isActive())
    {timer2->stop();}
    serial2->close();
    ui->state_2->setText(QString::fromLocal8Bit("串口断开"));
    ui->closeButton_2->setEnabled(false);
    ui->openButton_2->setEnabled(true);
    ui->freshButton_2->setEnabled(true);
}

void MainWindow::on_freshButton_2_clicked()
{
    ui->com_2->clear();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort seriala;
        seriala.setPort(info);
        if(seriala.open(QIODevice::ReadWrite))
        {
            ui->com_2->addItem(seriala.portName());
            seriala.close();
        }
    }
    ui->closeButton_2->setEnabled(true);
    ui->openButton_2->setEnabled(true);
}

void MainWindow::on_connectButton_2_clicked()
{
    if(timer2->isActive())
    {timer2->stop();}
    fire2=true;
    beginLaser2();
}

void MainWindow::on_disconnectButton_2_clicked()
{
    timer2->stop();
    closeLaser2();
}

void MainWindow::on_powerButton_2_clicked()
{
    powerSet2=false;
    QString toSend=ui->power_2->text();
//    int i=(toSend.split("uw").at(0)).toInt();
    int i=toSend.toInt();
    qDebug()<<i;
    setPower2(i);
}
/*void MainWindow::on_saveButton_clicked()
{
    mysetting.com1 = ui->com->currentText();
    mysetting.com2 = ui->com_2->currentText();
    mysetting.seedPower = ui->power->text().toInt();
    mysetting.pulsePower = ui->power_2->text().toInt();
    mySettingFile.WriteSetFile(mysetting,path);
}*/
