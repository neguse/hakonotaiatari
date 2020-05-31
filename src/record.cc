
#include "precompile.h"
#include "record.h"

#include "sqlite3_connector.h"

RecordTable::RecordTable()
	: m_pCon(NULL)
	, m_pInsertStatement(NULL)
{

}

RecordTable::~RecordTable()
{
	if (isInitialized()) {
		finalize();
	}
}

Retval RecordTable::initialize( Sqlite3Conector* pCon )
{
	if (isInitialized()) {
		return Retval(R_FAIL_MULTIPLE_INITIALIZE);
	}

	Sqlite3Statement* pCreateTableStmt = NULL;
	Sqlite3Statement* pInsertStmt = NULL;
	Sqlite3Statement* pMeanStmt = NULL;
	Sqlite3Statement* pVarianceStmt = NULL;
	Sqlite3Statement* pRankStmt = NULL;
	Sqlite3Statement* pTotalsStmt = NULL;
	Sqlite3Statement* pTopManyStmt = NULL;

	std::string query;
	query = "CREATE TABLE IF NOT EXISTS record_t "
		"(id INTEGER PRIMARY KEY ASC AUTOINCREMENT, "
		" score INTEGER, "
		" insert_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";
	pCreateTableStmt = pCon->CreateStatement(query.c_str());
	if (!pCreateTableStmt) {
		LOG(ERROR) << "Create�N�G����prepare�Ɏ��s���܂���";
		goto FAIL;
	}

	{
		Sqlite3StepResult res = pCreateTableStmt->step();
		if (res == STEP_RES_ERROR) {
			LOG(ERROR) << "Create�N�G���̎��s�Ɏ��s���܂���";
			goto FAIL;
		}
	}
	SAFE_DELETE(pCreateTableStmt);
	
	query = "INSERT INTO record_t (score) VALUES (?)";
	pInsertStmt = pCon->CreateStatement(query.c_str());
	if (!pInsertStmt) {
		LOG(ERROR) << "Insert�N�G����prepare�Ɏ��s���܂���";
		goto FAIL;
	}

	query = "SELECT avg(score) FROM record_t ";
	pMeanStmt = pCon->CreateStatement(query.c_str());
	if (!pMeanStmt) {
		LOG(ERROR) << "Mean�N�G����prepare�Ɏ��s���܂���";
		goto FAIL;
	}

	query = "SELECT AVG((score - ?) * (score - ?)) FROM record_t ";
	pVarianceStmt = pCon->CreateStatement(query.c_str());
	if (!pVarianceStmt) {
		LOG(ERROR) << "Variance�N�G����prepare�Ɏ��s���܂���";
		goto FAIL;
	}

	query = "SELECT COUNT(score) FROM record_t where score > ?";
	pRankStmt = pCon->CreateStatement(query.c_str());
	if (!pRankStmt) {
		LOG(ERROR) << "Rank�N�G����prepare�Ɏ��s���܂���";
		goto FAIL;
	}

	query = "SELECT count(score) FROM record_t ";
	pTotalsStmt = pCon->CreateStatement(query.c_str());
	if (!pTotalsStmt) {
		LOG(ERROR) << "Total�N�G����prepare�Ɏ��s���܂���";
		goto FAIL;
	}

	query = "SELECT id, score, datetime(insert_at, 'localtime') FROM record_t "
		"ORDER BY score DESC, id DESC LIMIT ?";
	pTopManyStmt = pCon->CreateStatement(query.c_str());
	if (!pTopManyStmt) {
		LOG(ERROR) << "TopMany�N�G����prepare�Ɏ��s���܂���";
		goto FAIL;
	}

	m_pCon = pCon;
	m_pInsertStatement = pInsertStmt;
	m_pMeanStatement = pMeanStmt;
	m_pVarianceStatement = pVarianceStmt;
	m_pRankStatement = pRankStmt;
	m_pTotalStatement = pTotalsStmt;
	m_pTopManyStatement = pTopManyStmt;

	return Retval(R_SUCCESS);

FAIL:
	
	SAFE_DELETE(pInsertStmt);
	SAFE_DELETE(pCreateTableStmt);
	SAFE_DELETE(pMeanStmt);
	SAFE_DELETE(pVarianceStmt);
	SAFE_DELETE(pRankStmt);
	SAFE_DELETE(pTotalsStmt);
	SAFE_DELETE(pTopManyStmt);

	return Retval(R_FAIL);
}

Retval RecordTable::finalize()
{
	if (!isInitialized()) {
		return Retval(R_FAIL_MULTIPLE_FINALIZE);
	}

	SAFE_DELETE(m_pInsertStatement);
	SAFE_DELETE(m_pMeanStatement);
	SAFE_DELETE(m_pVarianceStatement);
	SAFE_DELETE(m_pRankStatement);
	SAFE_DELETE(m_pTotalStatement);
	SAFE_DELETE(m_pTopManyStatement);
	return Retval(R_SUCCESS);
}

bool RecordTable::isInitialized() const
{
	return m_pCon != NULL;
}

bool RecordTable::insertRecord( RecordData& record )
{
	m_pInsertStatement->reset();
	m_pInsertStatement->bindInt64(1, record.score);
	Sqlite3StepResult ret = m_pInsertStatement->step();
	if (ret == STEP_RES_ERROR) {
		LOG(ERROR) << "step()�Ɏ��s���܂���";
		return false;
	}

	record.id = m_pInsertStatement->getLastInsertId();

	return true;
}

bool RecordTable::getRecordCalcData( const RecordData& record, RecordCalcData& outData )
{
	m_pRankStatement->reset();
	m_pRankStatement->bindInt64(1, record.score);
	{
		Sqlite3StepResult res = m_pRankStatement->step();
		if (res == STEP_RES_ERROR) {
			LOG(ERROR) << "RankStatement��step�Ɏ��s���܂���";
			return false;
		}

		outData.rank = m_pRankStatement->fetchInt64(0) + 1;
	}

	m_pMeanStatement->reset();
	{
		Sqlite3StepResult res = m_pMeanStatement->step();
		if (res == STEP_RES_ERROR) {
			LOG(ERROR) << "MeanStatement��step�Ɏ��s���܂���";
			return false;
		}

		outData.mean = m_pMeanStatement->fetchDouble(0);
	}

	f64 variance = 0.0;
	m_pVarianceStatement->reset();
	m_pVarianceStatement->bindInt64(1, outData.mean);
	m_pVarianceStatement->bindInt64(2, outData.mean);
	{
		Sqlite3StepResult res = m_pVarianceStatement->step();
		if (res == STEP_RES_ERROR) {
			LOG(ERROR) << "VarianceStatement��step�Ɏ��s���܂���";
			return false;
		}

		variance = m_pVarianceStatement->fetchDouble(0);
	}

	m_pTotalStatement->reset();
	{
		Sqlite3StepResult res = m_pTotalStatement->step();
		if (res == STEP_RES_ERROR) {
			LOG(ERROR) << "TotalStatement��step�Ɏ��s���܂���";
			return false;
		}

		outData.total = m_pTotalStatement->fetchInt64(0);
	}

	f64 sqrt_variance = sqrt(variance);
	f64 d_from_mean = record.score - outData.mean;
	if (sqrt_variance < 0.0001) {
		outData.standard = 50.0;
	} else {
		outData.standard = 50.0 + d_from_mean * 10.0 / sqrt_variance;
	}

	return true;
}

bool RecordTable::getTopRecord( int limit, std::vector<RecordData>& outList )
{
	outList.clear();
	m_pTopManyStatement->reset();
	m_pTopManyStatement->bindInt(1, limit);
	while (true) {
		Sqlite3StepResult res = m_pTopManyStatement->step();
		if (res == STEP_RES_ERROR) {
			LOG(ERROR) << "TopManyStatement��step�Ɏ��s���܂���";
			return false;
		}

		if (res == STEP_RES_DONE) {
			break;
		}

		RecordData r;
		r.id = m_pTopManyStatement->fetchInt64(0);
		r.score = m_pTopManyStatement->fetchInt64(1);
		r.inserted_at = std::string(m_pTopManyStatement->fetchText(2));
		outList.push_back(r);
	}

	return true;
}

