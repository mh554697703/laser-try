#ifndef LASERTRYSETTING_H
#define LASERTRYSETTING_H

#include <QString>
typedef struct
{
    //串口号
    QString com1;
    QString com2;
    //激光功率
    int seedPower;
    int pulsePower;
}LASERTRYSETTING;

#endif // LASERTRYSETTING_H
