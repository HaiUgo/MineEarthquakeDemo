#include "screenconfigure.h"

int ScreenConfigure::width = 0;

int ScreenConfigure::height = 0;

int ScreenConfigure::mwidth = 0;

int ScreenConfigure::mheight = 0;


bool ScreenConfigure::getScreenConfigure()
{
    QString path = QDir::currentPath();
    QString tempPath;
    tempPath=path + "/config/screenconfig.json";
    qDebug()<<"json file path"<<tempPath;
    QFile file(tempPath);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString value = file.readAll();
    file.close();

    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(), &parseJsonErr);
    if (!(parseJsonErr.error == QJsonParseError::NoError)) {
        qDebug()<<"json pares error";
        return false;
    }

    QJsonObject jsonObject = document.object();

    if (jsonObject.contains(QStringLiteral("Width"))){
        width = jsonObject["Width"].toString().toInt();
    }
    if (jsonObject.contains(QStringLiteral("Height"))){
        height = jsonObject["Height"].toString().toInt();
    }

    if (jsonObject.contains(QStringLiteral("mWidth"))){
        mwidth = jsonObject["mWidth"].toString().toInt();
    }
    if (jsonObject.contains(QStringLiteral("mHeight"))){
        mheight = jsonObject["mHeight"].toString().toInt();
    }
    qDebug()<<"width = "<<width <<"height = "<<height <<"mwidth = "<<mwidth <<"mheight = "<<mheight;
    return true;
}
