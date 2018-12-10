#ifndef SETTINGFILE_H
#define SETTINGFILE_H
#include <QString>
#include "lasertrysetting.h"

class settingfile
{
public:
    settingfile();

    //≈‰÷√Œƒº˛
    LASERTRYSETTING GetSetting();
    void CreatSetFile(const QString &a);
    void WriteSetFile(const LASERTRYSETTING &currentSetting, const QString &a);
    void ReadSetFile(const QString &b);
    bool SettingChanged(const LASERTRYSETTING &currentSetting);
private:
    QString LFpath;
    LASERTRYSETTING Lsetting;
};

#endif // SETTINGFILE_H
