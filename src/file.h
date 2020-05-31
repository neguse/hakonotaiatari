
// TODO: パックドファイルと統合する
class FileData
{
private:
	char* m_pData;
	u32 m_size;

public:

	FileData(const char* path = NULL)
		: m_pData(NULL)
		, m_size(0)
	{
		if (NULL != path) {
			open(path);
		}
	}

	~FileData()
	{
		delete[] m_pData;
	}

	char* data() const
	{
		return m_pData;
	}

	u32 size() const
	{
		return m_size;
	}

	Retval open(const char* path)
	{
		if (m_pData != NULL) {
			return Retval(R_FAIL_MULTIPLE_INITIALIZE);
		}
		if (NULL == path) {
			return Retval(R_FAIL);
		}

		std::ifstream is(path, std::ios::binary);
		if (is.fail()) {
			return Retval(R_FAIL_OPEN_FILE);
		}

		is.seekg (0, std::ios::end);
		m_size = is.tellg();
		is.seekg (0, std::ios::beg);

		if (m_size == 0) {
			return Retval(R_SUCCESS);
		}

		m_pData = new char[m_size];
		is.read(m_pData, m_size);

		return Retval(R_SUCCESS);
	}

}; // class FileData

