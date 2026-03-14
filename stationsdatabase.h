#ifndef STATIONSDATABASE_H
#define STATIONSDATABASE_H

#include <QListView>
#include <QMap>
#include <QString>
#include <QStringList>

class StationsDatabase : public QListView
{
    Q_OBJECT

public:
    StationsDatabase(QWidget *parent, QString filename = ":/DB_Haltestellen.csv");

    struct Data {
        double latitude = 0;
        double longitude = 0;
    };

    Data getDataForStation(QString name) const;

    void setModelWithStringList(const QStringList &strings);

private:
    QMap<QString, Data> stations;
};

#endif // STATIONSDATABASE_H
