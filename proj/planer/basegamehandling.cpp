#include "basegamehandling.h"

// base constructor
BaseGameHandling::BaseGameHandling(Database *db, QStringList *grPrefix)
{
    this->db = db;
    this->grPrefix = grPrefix;
}

// base destructor
BaseGameHandling::~BaseGameHandling()
{
}

// set time parameters
void BaseGameHandling::setTimeParameters(int satz, int min, int pause)
{
    this->satz = satz;
    this->min = min;
    this->pause = pause;
}

// clear all data
void BaseGameHandling::clearAllData(QStringList *tables)
{
    QStringList querys;

    for(int i = 0; i < tables->size(); i++)
        querys << "DELETE FROM " + tables->at(i);

    dbWrite(&querys);
}

// calculate results
void BaseGameHandling::calculateResult(QString round, QString resultTableName)
{
    emit logMessages("INFO:: calculating " + round + " results");
    QStringList execQuerys;
    QList<QStringList> vrGameResults = dbRead("SELECT spiel, ms_a, ms_b, satz1a, satz1b, satz2a, satz2b, satz3a, satz3b FROM " + round + " WHERE ms_a != '---' ORDER BY id ASC");
    QList<CalculateResults::teamResult> teamResults = CalculateResults::addResultsVrZw(CalculateResults::calculateResults(&vrGameResults));

    foreach(CalculateResults::teamResult tR, teamResults)
    {
        QString division;
        for(int i = 0; i < grPrefix->size();i ++)
        {
            QString prefix = grPrefix->at(i);

            if(dbRead("SELECT * FROM " + resultTableName + prefix + " WHERE ms = '" + tR.teamName + "'").count() > 0)
                division = prefix;
        }
        execQuerys << "UPDATE " + resultTableName + division + " SET punkte=" + QString::number(tR.sets) + ", satz=" + QString::number(tR.points) + " WHERE ms = '" + tR.teamName + "'";
    }

    dbWrite(&execQuerys);
}

//recalculate time schedule
void BaseGameHandling::recalculateTimeSchedule(QTableView *qtv, QSqlTableModel *model)
{
    QTime zeit = qtv->currentIndex().data().toTime();
    int addzeit = ((satz * min) + pause) * 60;
    int runde = model->data(model->index(qtv->currentIndex().row(), 1)).toInt();

    for(int i = qtv->currentIndex().row(); i <= model->rowCount(); i++)
    {
        if(runde != model->data(model->index(i, 1)).toInt())
        {
            zeit = zeit.addSecs(addzeit);
            runde++;
        }
        model->setData(model->index(i, 3), zeit.toString("hh:mm"));
    }
}

// insert field numbers
QStringList BaseGameHandling::insertFieldNr(QString round, int gameCount, int fieldCount)
{
    QStringList querys;

    for (int i = 1, field = 1; i <= gameCount; i++)
    {
        for(int x = 1, fieldHelp = field; x <= fieldCount; x++, fieldHelp++, i++)
        {
            querys << "UPDATE " + round + " SET feldnummer = " + QString::number(fieldHelp) + " WHERE id = " + QString::number(i);
            if(fieldHelp >= fieldCount)
                fieldHelp = 0;
        }

        i--;

        if(field < fieldCount)
            field++;
        else
            field = 1;
    }

    return querys;
}

// insert field names
QStringList BaseGameHandling::insertFieldNames(QString round, QStringList *fieldnames)
{
    QStringList querys;

    for(int i = 1; i <= fieldnames->size(); i++)
        querys << "UPDATE " + round + " SET feldname = '" + fieldnames->at(i-1) + "' WHERE feldnummer = " + QString::number(i);

    return querys;
}

// generate qualifying divisions result table
QStringList BaseGameHandling::generateResultTables(QString round, QList<QStringList> *divisionsList)
{
    QStringList querys;

    for(int i = 0, prefix = 0; i < divisionsList->size(); i++, prefix++)
    {
        QStringList division = divisionsList->at(i);
        QString group = grPrefix->at(prefix);

        for(int x = 0; x < division.size(); x++)
        {
            QString team = division.at(x);
            querys << "INSERT INTO " + round + group + " VALUES(" + QString::number(x) + ",'" + team + "',0,0,0,0)";
        }
    }

    return querys;
}

// check equal division results
QStringList BaseGameHandling::checkEqualDivisionResults(QString round, QString resultTableName)
{
    QStringList result;

    for(int i = 0; i < grPrefix->size(); i++)
    {
        QString prefix = grPrefix->at(i);

        QList<QStringList> getTeams = dbRead("select distinct ms1.ms from " + resultTableName
                                                   + prefix + " ms1, (select ms, satz, punkte, intern from " + resultTableName
                                                   + prefix + ") ms2 where ms1.satz = ms2.satz and  ms1.punkte = ms2.punkte and ms1.intern = ms2.intern and ms1.ms != ms2.ms");

        if(getTeams.count() == 2)
        {
            QStringList team1 = getTeams.at(0);
            QStringList team2 = getTeams.at(1);
            QString gamenr = db->read("SELECT spiel from " + round + " where ms_a = '"
                                         + team1.at(0) + "' and ms_b = '" + team2.at(0) + "' or ms_a = '"
                                         + team2.at(0) + "' and ms_b = '" + team1.at(0)+ "'").at(0).at(0);

            result.append("0");
            result.append(gamenr);
            result.append(team1.at(0));
            result.append(team2.at(0));
            return result;
        }
        else if(getTeams.count() > 2)
        {
            QStringList teams;

            teams << "1";

            foreach(QStringList team, getTeams)
                teams << team.at(0);

            return teams;
        }
    }

    return result;
}

// write to database with query list
void BaseGameHandling::dbWrite(QStringList *querys)
{
    for(int i = 0; i < querys->size(); i++)
        db->write(querys->at(i));
}

// write to database with single query
void BaseGameHandling::dbWrite(QString query)
{
    db->write(query);
}

// read prefix from list
QString BaseGameHandling::getPrefix(int index)
{
    return grPrefix->at(index);
}

// return prefix count
int BaseGameHandling::getPrefixCount()
{
    return grPrefix->size();
}

// parse int to string
QString BaseGameHandling::string(int val)
{
    return QString::number(val);
}

// read from database
QList<QStringList> BaseGameHandling::dbRead(QString query)
{
    return db->read(query);
}
