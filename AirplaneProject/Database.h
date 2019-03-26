#pragma once
#include "Headers.h"
#include "Airplane.h"
#include <sstream>

using namespace std;
using namespace std::chrono;
using namespace date;
class Database
{
public:
	_RecordsetPtr RecordSetPtr;
	_ConnectionPtr ConnectionPtr;
	
	Database(_bstr_t connectionString);
	~Database();
	void SaveData(time_point<system_clock> sessionTime, unique_ptr<vector<Airplane*>> &notDestroyed, unique_ptr<vector<Airplane*>> &Destroyed);
	vector<_bstr_t> GetData(time_point<system_clock> sessionTime);
private:
	_bstr_t connectionString;
	bool connectionOpened;
	bool OpenConnection();
	void CloseConnection();
	void PrintProviderError();
	string GetTimeDate(time_point<system_clock> time);
	string ToHexString(const unsigned int i);
	string ToColor(const unsigned int i);
};