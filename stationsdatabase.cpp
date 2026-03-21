#include "stationsdatabase.h"

#include <QFile>
#include <QMessageBox>
#include <QStringListModel>
#include <QTextStream>

StationsDatabase::StationsDatabase(QWidget *parent, QString filename) : QListView{parent}
{
    QFile databaseFile{filename};

    if (!databaseFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Stations database is not available"));
        return;
    }

    QStringList stationsNames;
    QTextStream inputStream{&databaseFile};

    // Get rid of the header column
    if (!inputStream.atEnd()) {
        static_cast<void>(inputStream.readLine());
    }

    while (!inputStream.atEnd()) {
        auto line = inputStream.readLine();
        auto columns = line.split(";");

        // TODO: Get rid of the hardcoded numbers
        auto name = columns[4];

        stationsNames << name;

        // TODO: Get rid of the hardcoded numbers
        auto longitude = columns[6].toDouble();
        auto latitude = columns[7].toDouble();
        stations[name] = Data{latitude, longitude};
    }

    databaseFile.close();

    stationsNames.sort();

    setModelWithStringList(stationsNames);
}

StationsDatabase::Data StationsDatabase::getDataForStation(QString name) const
{
    auto iter = stations.find(name);
    return iter != stations.end() ? *iter : StationsDatabase::Data{};
}

void StationsDatabase::setModelWithStringList(const QStringList &strings)
{
    QStringListModel *listModel = new QStringListModel{this};

    listModel->setStringList(strings);

    setModel(listModel);
}
