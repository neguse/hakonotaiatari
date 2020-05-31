
#pragma once

class Sqlite3Conector;
class Sqlite3Statement;

struct RecordData
{
	i64 id;
	i64 score;
	std::string inserted_at;
};

struct RecordCalcData
{
	i64 rank;
	f64 mean;
	f64 standard;
	i64 total;
};

class RecordTable
{
public:

	RecordTable();
	virtual ~RecordTable();

	Retval initialize(Sqlite3Conector* pCon);
	Retval finalize();
	bool isInitialized() const;

	bool insertRecord(RecordData& record);
	int getRecordCount();
	bool getRecordCalcData(const RecordData& record, RecordCalcData& outData);
	bool getTopRecord(int limit, std::vector<RecordData>& outList);

private:
	Sqlite3Conector* m_pCon;
	Sqlite3Statement* m_pInsertStatement;
	Sqlite3Statement* m_pMeanStatement;
	Sqlite3Statement* m_pVarianceStatement;
	Sqlite3Statement* m_pRankStatement;
	Sqlite3Statement* m_pTotalStatement;
	Sqlite3Statement* m_pTopManyStatement;

}; // class RecordTable