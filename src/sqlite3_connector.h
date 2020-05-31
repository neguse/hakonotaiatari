
#pragma once

struct sqlite3;
struct sqlite3_stmt;

enum Sqlite3StepResult
{
	STEP_RES_DONE,
	STEP_RES_ROW,
	STEP_RES_ERROR,
};

class Sqlite3Statement
{
public:

	Sqlite3Statement(sqlite3* pCon, sqlite3_stmt* pStatement);
	virtual ~Sqlite3Statement();

	bool reset();

	Sqlite3StepResult step();
	bool bindInt(int index, int value);
	bool bindInt64(int index, i64 value);
	bool bindDouble(int index, f64 value);

	i32 fetchInt(int index);
	i64 fetchInt64(int index);
	f64 fetchDouble(int index);
	const char* fetchText(int index);

	i64 getLastInsertId();

private:
	sqlite3_stmt* m_pStatement;
	sqlite3* m_pDatabase;
};

class Sqlite3Conector
{
public:

	Sqlite3Conector();
	virtual ~Sqlite3Conector();

	Retval initialize(const char* path);
	Retval finalize();
	bool isInitialized() const
	{
		return m_pDatabase != NULL;
	}

	Sqlite3Statement* CreateStatement(const char* pQuery);

private:

	sqlite3* m_pDatabase;

}; // class Sqlite3Connector
