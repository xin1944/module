#include <iostream>
#include <string>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "ice_database.h"

using namespace std;

int main()
{
	string test = "my test string\n";
	cout<<test;
	return 0;
}

int haveTable()
{
    //for dm7
#if 0
    std::string result = "";
    QString ss = "select name from sys.sysobjects where name like 'SG_PRT%B' and schid in"
        "(select id from sys.sysobjects where name='FAULTALL' and TYPE$='SCH')";
    if(execQuery(ss.toStdString().c_str(), result))
    {
        QJsonParseError err_rpt;
        QJsonDocument  root_Doc = QJsonDocument::fromJson(result.c_str(), &err_rpt);
        if(err_rpt.error != QJsonParseError::NoError)
        {
            qDebug() << "JSON格式错误";
            return -1;
        }
        else
        {
            qDebug() << result.c_str();
            QJsonArray jroot = root_Doc.array();
            for(int i = 0;i<jroot.size();i++)
            {
                QJsonObject id_obj = jroot.at(i).toObject();
                QString tb = id_obj.value("NAME").toString();
                QString selsql = QObject::tr("select id,name from faultall.%1").arg(tb);
                std::string res2 = "";
                if(execQuery(selsql.toStdString().c_str(), res2))
                {
                    QJsonParseError err_rpt2;
                    QJsonDocument  root_Doc2 = QJsonDocument::fromJson(res2.c_str(), &err_rpt2);
                    if(err_rpt2.error != QJsonParseError::NoError)
                    {
                        qDebug() << "JSON格式错误";
                        return -1;
                    }
                    else
                    {
                        qDebug() << res2.c_str();
                        QJsonArray jroot2 = root_Doc2.array();
                        for(int j = 0;j<jroot2.size();j++)
                        {
                        QJsonObject id_obj2 = jroot2.at(j).toObject();
                        QString name = id_obj2.value("NAME").toString();
                        QString id = id_obj2.value("ID").toString();
                        QString sql = QObject::tr("update faultall.IED set PRT_DEV_ID='%1' WHERE IED='%2'")
                                                .arg(id).arg(name);
                        execNoQuery(sql.toStdString().c_str());
                        }
                    }
                }
                else
                    qDebug()<<"select id,name fail from"<<tb;
            }
        }
    }
    else
        qDebug()<<"select table fail";
#endif
    return 0;
}
#if 0
    
    QFile file("bayied.json");
    file.open(QIODevice::ReadOnly);
    QByteArray ba= file.readAll();
    QJsonParseError err_rpt;
    QJsonDocument  root_Doc = QJsonDocument::fromJson(ba, &err_rpt);
    if(err_rpt.error != QJsonParseError::NoError)
    {
        qDebug() << err_rpt.errorString();
        return -1;
    }
    else
    {
        QJsonArray jroot = root_Doc.array();
        for(int i = 0;i<jroot.size();i++)
        {
            QJsonObject id_obj = jroot.at(i).toObject();
            QString ied = id_obj.value("IED_NAME").toString();
            double fx = id_obj.value("F_X").toDouble();
            double fy = id_obj.value("F_Y").toDouble();
            QString sfx = QString::number(fx, 'f', 13);
            QString sfy = QString::number(fy, 'f', 13);
            QString sql = QObject::tr("update faultall.bay_ied set f_x='%1',f_y='%2' WHERE IED_NAME='%3'")
                                    .arg(sfx).arg(sfy).arg(ied);
            //qDebug()<<QString::number(fx, 'f', 13);
            execNoQuery(sql.toStdString().c_str());
        }
    }
#endif