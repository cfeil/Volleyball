#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QFile>

#include "database.h"
#include "logging.h"
#include "ftploader.h"
#include "calculateresults.h"
#include "qualifyinggames.h"
#include "interimgames.h"
#include "crossgames.h"
#include "classementgames.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    typedef struct
    {
        QString sysFilePath;
        QString pdfPath;
        int anzFelder;
        int krSpiele;
        int bettySpiele;
        QString startTurnier;
        int pauseVrZw;
        int pauseZwKr;
        int pauseKrPl;
        int satzVr;
        int minSatzVr;
        int pauseMinVr;
        int satzZw;
        int minSatzZw;
        int pauseMinZw;
        int satzKr;
        int minSatzKr;
        int pauseMinKr;
        int satzPl;
        int minSatzPl;
        int zeitFinale;
        int pausePlEhrung;
    } dataUi;

    explicit Worker(QString *settingsFile, QString *dbFile, QString *logFile,
                    QStringList *qfTablesToClear, QStringList *itTablesToClear,
                    QStringList *crTablesToClear, QStringList *clTablesToClear,
                    QStringList *insertRows, QStringList *grPrefix, QStringList *headerPrefix, QObject *parent = 0);

    // teams
    bool checkDoubleTeamNames(QSqlTableModel *model);
    void resetTeams();
    int getTeamsCount();
    int getDivisionsCount();
    void uploadFile();

    // fields
    void setFieldsTableRows(int spinBoxCount);
    QStringList getFieldNames();

    // data ui <=> worker
    void updateUiData();
    void resetConfig();

    // db
    QSqlQueryModel* createSqlQueryModel(QString query);
    QSqlTableModel* createSqlTableModel(QString tableName, QStringList *columnName);
    bool commitSqlTableModel(QSqlTableModel *model);

    // qualifying games
    int getQualifyingGamesCount();
    void setParametersQualifyingGames();
    void generateQualifyingGames();
    void clearQualifyingGames();
    void calculateQualifyingGames();
    void recalculateQualifyingGamesTimeSchedule(QTableView *qtv, QSqlTableModel *tm);
    QStringList checkEqualDivisionResults();
    QString getQualifyingGamesMaxTime();

    // interim games
    void setParametersInterimGames();
    bool generateInterimGames();
    void clearInterimGames();
    void calculateInterimGames();
    void recalculateInterimGamesTimeSchedule(QTableView *qtv, QSqlTableModel *tm);
    int getInterimGamesCount();
    QString getInterimGamesMaxTime();

    // cross games
    void setParametersCrossGames();
    void generateCrossGames();
    void clearCrossGames();
    void recalculateCrossGamesTimeSchedule(QTableView *qtv, QSqlTableModel *tm);
    int getCrossGamesCount();
    QString getCrossGamesMaxTime();

    // classement games
    void setParametersClassementGames();
    void generateClassementGames();
    void clearClassementGames();
    void recalculateClassementGamesTimeSchedule(QTableView *qtv, QSqlTableModel *tm);
    int getClassementGamesCount();
    void getFinalClassement();
    QString getClassementGamesMaxTime();

signals:
    void criticalMessage(QString message);
    void warningMessage(QString message);
    void infoMessage(QString message);
    void log(QString);
    void updateUi(Worker::dataUi*);

public slots:
    void updateWorkerData(Worker::dataUi *data);
    void logging(QString message);

private:
    void init();
    void readConfig();
    void writeConfig(int anzfelder, int kreuzspiele, int bettyspiele, QString startturnier, int pausevrzw, int pausezwkr, int pausekrpl,
                     int satzvr, int minsatzvr, int pauseminvr, int satzzw, int minsatzzw, int pauseminzw, int satzkr,
                     int minsatzkr, int pauseminkr, int satzpl, int minsatzpl, int zeitfinale, int pauseplehrung);

    QStringList *qfTablesToClear, *itTablesToClear, *crTablesToClear, *clTablesToClear;
    QStringList *insertRows, *grPrefix, *headerPrefix;
    QString *settingsFile, *dbFile, *logFile;

    Logging *logs;
    Database *db;
	FTPLoader *ftpload;
    CalculateResults *cr;
    QualifyingGames *qf;
    InterimGames *im;
    CrossGames *cg;
    ClassementGames *clg;
    dataUi *data;

    int teamsCount, divisionCount;
    QStringList fieldNames;
};

#endif // WORKER_H
