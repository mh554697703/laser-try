#include "settingfile.h"
#include "ui_mainwindow.h"
#include<QFile>
#include<QSettings>
#include<QDebug>

settingfile::settingfile()
{

}

void settingfile::CreatSetFile(const QString &a)
{
    QFile file(a);
    QSettings settings(a,QSettings::IniFormat);
    if(file.exists() == false)
    {
        settings.beginGroup("lasertry");
        settings.setValue("com1","COM1");
        settings.setValue("com2","COM2");
        settings.setValue("seedPower",1000);
        settings.setValue("pulsePower",1000);
        settings.endGroup();
    }
    else
    {
        qDebug() <<"Settings file exist";
    }
    QFile LFfile(a);
    if(LFfile.exists() == false)
    {
        LFfile.open(QIODevice::WriteOnly | QIODevice::Truncate);
        LFfile.close();
    }
}

void settingfile::ReadSetFile(const QString &b)
{
    QSettings setting(b,QSettings::IniFormat);
    setting.beginGroup("lasertry");
    Lsetting.com1 = setting.value("com1").toString();
    Lsetting.com2 = setting.value("com2").toString();
    Lsetting.seedPower = setting.value("seedPower").toInt();
    Lsetting.pulsePower = setting.value("pulsePower").toInt();
    setting.endGroup();
}

LASERTRYSETTING settingfile::GetSetting()
{
    return Lsetting;
}

void settingfile::WriteSetFile(const LASERTRYSETTING &currentSetting, const QString &a)
{
    Lsetting = currentSetting;
    QSettings setting(a,QSettings::IniFormat);
    setting.beginGroup("lasertry");
    setting.setValue("com1",Lsetting.com1);
    setting.setValue("com2",Lsetting.com2);
    setting.setValue("seedPower",Lsetting.seedPower);
    setting.setValue("pulsePower",Lsetting.pulsePower);
    setting.endGroup();
}

bool settingfile::SettingChanged(const LASERTRYSETTING &currentSetting)
{
    LASERTRYSETTING newSetting = currentSetting;
    if(newSetting.com1 != Lsetting.com1)
        return true;
    if(newSetting.com2 != Lsetting.com2)
        return true;
    if(newSetting.seedPower != Lsetting.seedPower)
        return true;
    if(newSetting.pulsePower != Lsetting.pulsePower)
        return true;
    return false;
}
