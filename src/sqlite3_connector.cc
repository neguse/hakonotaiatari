
#include "precompile.h"
#include "sqlite3_connector.h"

#include "sqlite3/sqlite3.h"

#define TRACE_SQLITE3ERR 	LOG(ERROR) << sqlite3_errmsg(m_pDatabase)

Sqlite3Statement::Sqlite3Statement( sqlite3* pCon, sqlite3_stmt* pStatement )
: m_pDatabase(pCon)
, m_pStatement(pStatement)
{
	reset();
}

Sqlite3Statement::~Sqlite3Statement()
{
	int ret = sqlite3_finalize(m_pStatement);
	if (ret != SQLITE_OK) {
		TRACE_SQLITE3ERR;
	}
}

bool Sqlite3Statement::reset()
{
	int ret = sqlite3_reset(m_pStatement);
	if (ret != SQLITE_OK) {
		TRACE_SQLITE3ERR;
		return false;
	}
	return true;
}

Sqlite3StepResult Sqlite3Statement::step()
{
	int ret = sqlite3_step(m_pStatement);
	if (ret == SQLITE_DONE) {
		return STEP_RES_DONE;
	} else if (ret == SQLITE_ROW) {
		return STEP_RES_ROW;
	} else {
		TRACE_SQLITE3ERR;
		return STEP_RES_ERROR;
	}
}

bool Sqlite3Statement::bindInt( int index, int value )
{
	int ret = sqlite3_bind_int(m_pStatement, index, value);
	if (ret != SQLITE_OK) {
		TRACE_SQLITE3ERR;
		return false;
	}

	return true;
}

bool Sqlite3Statement::bindInt64( int index, i64 value )
{
	int ret = sqlite3_bind_int64(m_pStatement, index, value);
	if (ret != SQLITE_OK) {
		TRACE_SQLITE3ERR;
		return false;
	}

	return true;
}

bool Sqlite3Statement::bindDouble( int index, f64 value )
{
	int ret = sqlite3_bind_double(m_pStatement, index, value);
	if (ret != SQLITE_OK) {
		TRACE_SQLITE3ERR;
		return false;
	}

	return true;
}

i32 Sqlite3Statement::fetchInt( int index )
{
	i32 ret = sqlite3_column_int(m_pStatement, index);
	return ret;

}

i64 Sqlite3Statement::fetchInt64( int index )
{
	i32 ret = sqlite3_column_int64(m_pStatement, index);
	return ret;
}

f64 Sqlite3Statement::fetchDouble( int index )
{
	f64 ret = sqlite3_column_double(m_pStatement, index);
	return ret;
}

const char* Sqlite3Statement::fetchText( int index )
{
	const char* ret = (const char*)sqlite3_column_text(m_pStatement, index);
	return ret;
}

i64 Sqlite3Statement::getLastInsertId()
{
	return sqlite3_last_insert_rowid(m_pDatabase);
}

Sqlite3Conector::Sqlite3Conector()
	: m_pDatabase(NULL)
{

}

Sqlite3Conector::~Sqlite3Conector()
{
	if (isInitialized()) {
		finalize();
	}
}

Retval Sqlite3Conector::initialize( const char* path )
{
	if (isInitialized()) {
		return Retval(R_FAIL_MULTIPLE_INITIALIZE);
	}

	char buf[MAX_PATH];
	wchar_t wBuf[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, path, -1, wBuf, MAX_PATH);
	u32 length = ::WideCharToMultiByte(CP_UTF8, 0, wBuf, -1, buf, MAX_PATH, NULL, NULL);
	LOG(INFO) << "DBファイルパス: " << buf;


	int ret = sqlite3_open_v2(buf, &m_pDatabase, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	if (ret != SQLITE_OK) {
		TRACE_SQLITE3ERR;
		return Retval(R_FAIL);
	}
	return Retval(R_SUCCESS);
}

Retval Sqlite3Conector::finalize()
{
	if (!isInitialized()) {
		return Retval(R_FAIL_MULTIPLE_FINALIZE);
	}

	int ret = sqlite3_close(m_pDatabase);
	if (ret != SQLITE_OK) {
		TRACE_SQLITE3ERR;
		return Retval(R_FAIL);
	}
	m_pDatabase = NULL;

	return Retval(R_SUCCESS);
}

Sqlite3Statement* Sqlite3Conector::CreateStatement( const char* pQuery )
{
	sqlite3_stmt* pStatement = NULL;
	int ret = sqlite3_prepare_v2(
		m_pDatabase,
		pQuery, strlen(pQuery),
		&pStatement, NULL);
	if (ret != SQLITE_OK) {
		LOG(ERROR) << "in query \"" << pQuery << "\"";
		TRACE_SQLITE3ERR;
		return NULL;
	}

	return new Sqlite3Statement(m_pDatabase, pStatement);
}

