﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Volleyball
{
    [Serializable]
    class BaseGameHandling
    {
        #region members
        Logging log;
        int setCounter;
        int minutes;
        int pause;
        int[] rotationArray;
        public List<MatchData> matchData;
        public List<List<ResultData>> resultData;
        #endregion

        public BaseGameHandling(Logging log)
        {
            this.log = log;
            matchData = new List<MatchData>();
            resultData = new List<List<ResultData>>();
        }       

        public void setTimeParameters(int setCounter, int minutes, int pause)
        {
            this.setCounter = setCounter;
            this.minutes = minutes;
            this.pause = pause;
        }

        public void createRotationArray(int count)
        {
            rotationArray = new int[count];

            for (int i = 0; i < count; i++)
                rotationArray[i] = i + 1;
        }

        public void shiftRight(int[] list)
        {
            if (list.Length < 2)
                return;

            int oldFirst = list[0];

            for (int i = 0; i < (list.Length - 1); i++)
                list[i] = list[i + 1];

            list[list.Length - 1] = oldFirst;
        }

        // calculate results
        public void calculateResults()
        {
            List<TeamResult> teamResults = CalculateResults.addResultsForQualifyingAndInterimRounds(CalculateResults.calculateResults(matchData));

            foreach (TeamResult teamResult in teamResults)
            {
                foreach(List<ResultData> resultDataList in resultData)
                {
                    foreach(ResultData resultDataTeam in resultDataList)
                    {
                        if(resultDataTeam.Team == teamResult.TeamName)
                        {
                            resultDataTeam.PointsSets = teamResult.PointsSet;
                            resultDataTeam.PointsMatches = teamResult.PointsMatch;
                            break;
                        }
                    }
                }
            }
        }

        public void insertGameTime(DateTime startTournament)
        {
            int addtime = ((setCounter * minutes) + pause) * 60;

            for (int i = 0, lastRound = 0; i < matchData.Count; i++)
            {
                int roundValue = matchData[i].Round;

                if (lastRound < roundValue)
                {
                    lastRound = roundValue;
                    startTournament = startTournament.AddSeconds(addtime);
                }

                matchData[i].Time = startTournament;                
            }
        }

        //recalculate time schedule
        public void recalculateTimeSchedule(int changeIndex, DateTime gameTime)
        {
            int round = matchData[changeIndex].Round;
            int addTime = ((setCounter * minutes) + pause) * 60;
            
            for (int i = changeIndex; i < matchData.Count; i++)
            {
                if (round != matchData[i].Round)
                {
                    gameTime = gameTime.AddSeconds(addTime);
                    round++;
                }

                matchData[i].Time = gameTime;
            }
        }

        public void insertGameNumber()
        {
            for (int i = 0; i < matchData.Count; i++)
                matchData[i].Game = i + 1;
        }

        public void insertRoundNumber(int teamsCount, int fieldCount)
        {
            for (int i = 0, ii = 1, round = 1; i < matchData.Count; i++)
            {
                matchData[i].Round = round;

                if (ii < fieldCount)
                {
                    ii++;
                }
                else
                {
                    ii = 1;
                    round++;
                }
            }
        }

        // insert field numbers
        public void insertFieldnumbersAndFieldnames(int fieldCount, List<String> fieldNames)
        {
            int field = 0;

            createRotationArray(fieldCount);

            foreach(MatchData md in matchData)
            {
                md.FieldNumber = rotationArray[field];
                md.FieldName = fieldNames[rotationArray[field] - 1];

                if (field < rotationArray.Length - 1)
                {
                    field++;
                }
                else
                {
                    field = 0;
                    shiftRight(rotationArray);
                }
            }
        }       

        // generate divisions result lists
        public void fillResultLists(List<List<String>> divisionsList)
        {
            resultData.Clear();

            foreach (List<String> divisionList in divisionsList)
            {
                List<ResultData> teamList = new List<ResultData>();
                foreach (String team in divisionList)
                    teamList.Add(new ResultData(team));

                resultData.Add(teamList);
            }
        }

        // check equal division results
        public List<String> checkEqualDivisionResults()
        {
            List<String> result = new List<String>();

            for (int i = 0; i < resultData.Count; i++)
            {
                /*List<List<String>> getTeams = readDatabase("select distinct ms1.ms from " + resultTableName
                                                           + prefix + " ms1, (select ms, satz, punkte, intern from " + resultTableName
                                                           + prefix + ") ms2 where ms1.satz = ms2.satz and  ms1.punkte = ms2.punkte and ms1.intern = ms2.intern and ms1.ms != ms2.ms");

                if (getTeams.Count == 2)
                {
                    List<String> team1 = getTeams[0];
                    List<String> team2 = getTeams[1];
                    String gamenumber = readDatabase("SELECT spiel from " + round + " where ms_a = '"
                                                 + team1[0] + "' and ms_b = '" + team2[0] + "' or ms_a = '"
                                                 + team2[0] + "' and ms_b = '" + team1[0] + "'")[0][0];

                    result.Add("0");
                    result.Add(gamenumber);
                    result.Add(team1[0]);
                    result.Add(team2[0]);
                    return result;
                }
                else if (getTeams.Count > 2)
                {
                    List<String> teams = new List<String>();

                    teams.Add("1");

                    foreach (List<String> team in getTeams)
                        teams.Add(team[0]);

                    return teams;
                }*/
            }

            return result;
        }

        public void sortResults()
        {
            foreach(List<ResultData> rd in resultData)
            {
                if (rd != null)
                {
                    for (int i = rd.Count - 1; i > 0; i--)
                    {
                        for (int ii = 0; ii < i; ii++)
                        {
                            if (rd[ii].PointsSets < rd[ii + 1].PointsSets)
                            {
                                ResultData team = rd[ii];

                                rd[ii] = rd[ii + 1];
                                rd[ii + 1] = team;
                            }
                            else if (rd[ii].PointsSets == rd[ii + 1].PointsSets 
                                && rd[ii].PointsMatches < rd[ii + 1].PointsMatches)
                            {
                                ResultData team = rd[ii];

                                rd[ii] = rd[ii + 1];
                                rd[ii + 1] = team;
                            }
                            else if (rd[ii].PointsSets == rd[ii + 1].PointsSets 
                                && rd[ii].PointsMatches == rd[ii + 1].PointsMatches
                                && rd[ii].InternalRank < rd[ii + 1].InternalRank)
                            {
                                ResultData team = rd[ii];

                                rd[ii] = rd[ii + 1];
                                rd[ii + 1] = team;
                            }
                        }
                    }

                    for (int x = 0; x < rd.Count; x++)
                        rd[x].Rank = x + 1;
                }
            }
        }
    }
}