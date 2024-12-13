#include "idatabase.h"
#include<QUuid>
void IDatabase::initDatabase()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    QString aFile = "D:/study/project/Qt/Lab3/lab3.db";
    database.setDatabaseName(aFile);
    if (!database.open()) {
        qDebug() << "failed to open database";
    } else {
        qDebug() << "open database is ok";
    }
}

bool IDatabase::initPatientModel()
{
    PatientTabModel = new QSqlTableModel(this, database);
    PatientTabModel->setTable("Patient");
    PatientTabModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    PatientTabModel->setSort(PatientTabModel->fieldIndex("NAME"), Qt::AscendingOrder);
    if (!(PatientTabModel->select()))
        return false;
    thePatientSelection = new QItemSelectionModel(PatientTabModel);
    return true;
}

bool IDatabase::searchPatient(QString filter)
{
    PatientTabModel->setFilter(filter);
    return PatientTabModel->select();
}

int IDatabase::addNewPatient()
{
    PatientTabModel->insertRow(PatientTabModel->rowCount(), QModelIndex());
    QModelIndex curIndex = PatientTabModel->index(PatientTabModel->rowCount() - 1, 1);
    int curRecNo = curIndex.row();

    QSqlRecord curRec = PatientTabModel->record(curRecNo);
    curRec.setValue("CREATEDTIMESTAMP", QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    curRec.setValue("ID", QUuid::createUuid().toString(QUuid::WithoutBraces));
    PatientTabModel->setRecord(curRecNo, curRec);
    return curIndex.row();
}

bool IDatabase::deleteCurrentPatient()
{
    QModelIndex curIndex = thePatientSelection->currentIndex();
    PatientTabModel->removeRow(curIndex.row());
    PatientTabModel->submitAll();
    PatientTabModel->select();
}

bool IDatabase::submitPatientEdit()
{
    return PatientTabModel->submitAll();
}

void IDatabase::revertPatientEdit()
{
    PatientTabModel->revertAll();
}

QString IDatabase::userLogin(QString userName, QString password)
{
    QSqlQuery query;
    query.prepare("select USERNAME,PASSWORD from user where username=:USER");
    query.bindValue(":USER", userName);
    query.exec();
    query.first();
    if (query.first() && query.value("USERNAME").isValid()) {
        QString passwd = query.value("PASSWORD").toString();
        if (passwd == password) {

            return "LoginOk";
        } else {
            return "wrong password";
        }
    } else {
        qDebug() << "no such user";
        return "wrong username";
    }
}

IDatabase::IDatabase(QObject *parent)
    : QObject{parent}
{
    initDatabase();
}
