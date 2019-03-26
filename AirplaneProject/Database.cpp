#include "Database.h"



Database::Database(_bstr_t connectionString) :connectionString(connectionString)
{
	ConnectionPtr = NULL;
	RecordSetPtr = NULL;
	connectionOpened = false;


	
}

Database::~Database()
{	
	
	//CloseConnection();
	cout << "Database Destroyed" << endl;
}

void Database::SaveData(time_point<system_clock> sessionTime, unique_ptr<vector<Airplane*>>& notDestroyed, unique_ptr<vector<Airplane*>>& Destroyed)
{
	if (!connectionOpened) {
		if (!OpenConnection()) {	
			cout << "OpenConnection() failed"<<endl;
			return;
		}
			
	}
	try {
		ConnectionPtr->BeginTrans();
		_variant_t recordsAffected;
		string time = GetTimeDate(sessionTime);
		for (Airplane *currentPlane : *notDestroyed.get()) {
			string planeCreationTime = GetTimeDate(currentPlane->getStartTime());
			string SqlQuerry = "INSERT INTO PlaneTable(SessionTime,PlaneID,BornTime,Color) VALUES('" + time + "','"+to_string(currentPlane->getId())+
			"','"+planeCreationTime+ "','" + ToColor(currentPlane->getColor()) + "');";
			cout << SqlQuerry << endl;
			ConnectionPtr->Execute(SqlQuerry.c_str(), &recordsAffected, adExecuteNoRecords);
		}
		for (Airplane *currentPlane : *Destroyed.get()) {
			string planeCreationTime = GetTimeDate(currentPlane->getStartTime());
			string planeDestroyTime = GetTimeDate(currentPlane->getEndTime());
			string SqlQuerry = "INSERT INTO PlaneTable(SessionTime,PlaneID,BornTime,DestructionTime,Color) VALUES('" + time + "','" + to_string(currentPlane->getId()) +
				"','" + planeCreationTime + "','"+planeDestroyTime+"','"+ToColor(currentPlane->getColor())+"');";
			cout << SqlQuerry << endl;
			ConnectionPtr->Execute(SqlQuerry.c_str(), &recordsAffected, adExecuteNoRecords);
		}
		ConnectionPtr->CommitTrans();
	}catch (_com_error &e) {
		// Notify the user of errors if any.  
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		PrintProviderError();

		printf("Source : %s\n", (LPCSTR)bstrSource);
		printf("Description : %s\n", (LPCSTR)bstrDescription);
	}
	catch (...) {
		printf("unknown exception : %s\n","SaveData");
	}
	CloseConnection();
	cout << "DATA SAVED" << endl;
	

}

vector<_bstr_t> Database::GetData(time_point<system_clock> sessionTime)
{
	vector<_bstr_t> output;
	if (!connectionOpened) {
		if (!OpenConnection()) {
			cout << "OpenConnection() failed" << endl;
			return output;
		}
	}
	try {
	string time = GetTimeDate(sessionTime);
	RecordSetPtr->Open("PlaneTable", _variant_t((IDispatch*)ConnectionPtr, true),adOpenDynamic, adLockPessimistic, adCmdTable);
	_variant_t results;
	string SqlQuerry = "SELECT * FROM PlaneTable WHERE SessionTime='" + time + "' ORDER BY PlaneID;";
	//string SqlQuerry = "SELECT * FROM PlaneTable;";
	RecordSetPtr = ConnectionPtr->Execute(SqlQuerry.c_str(), &results, adCmdText);
	_bstr_t id;
	_bstr_t bornTime;
	_bstr_t destructionTime;
	_bstr_t color;
	while (!RecordSetPtr->adoEOF) {
		//PlaneName = RecordSetPtr->Fields->GetItem("PlaneName")->Value;
		id = RecordSetPtr->Fields->GetItem("PlaneID")->Value;
		bornTime= RecordSetPtr->Fields->GetItem("BornTime")->Value;
		if (RecordSetPtr->Fields->GetItem("DestructionTime")->Value.vt != VT_NULL) {
			destructionTime = RecordSetPtr->Fields->GetItem("DestructionTime")->Value;
		}
			
		else
			destructionTime = "Not destroyed";
		color = RecordSetPtr->Fields->GetItem("Color")->Value;
		
		_bstr_t str ="ID=  "+id+"  Creation Time :" + bornTime + " ,  Destruction Time :  " + destructionTime + " , " + color;
		output.push_back(str);
		cout << str << endl;
		RecordSetPtr->MoveNext();
	}
	}catch (_com_error &e) {
		// Notify the user of errors if any.  
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		PrintProviderError();

		printf("Source : %s\n", (LPCSTR)bstrSource);
		printf("Description : %s\n", (LPCSTR)bstrDescription);
	}
	catch (...) {
		printf("unknown exception : %s\n", "GetData");
	}
	CloseConnection();
	return output;
}

bool Database::OpenConnection()
{	
	bool result = true;
	try {
		::CoInitialize(NULL);
		ConnectionPtr.CreateInstance(__uuidof(Connection));
		ConnectionPtr->Open(connectionString, "admin", "", adConnectUnspecified);
		RecordSetPtr.CreateInstance(__uuidof(Recordset));
		connectionOpened = true;
	}
	catch (_com_error &e) {
		// Notify the user of errors if any.  
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		PrintProviderError();

		printf("Source : %s\n", (LPCSTR)bstrSource);
		printf("Description : %s\n", (LPCSTR)bstrDescription);
		printf("FAILED TO OPEN CONNECTION");
		result = false;
	}
	catch (...) {
		printf("unknown exception : %s\n", "OpenConnection");
	}
	cout << "connection opened" << endl;
	return result;
}

void Database::CloseConnection()
{
	try {
		if (RecordSetPtr)
			if (RecordSetPtr->State == adStateOpen)
				RecordSetPtr->Close();
		cout << "record closed" << endl;
		if (ConnectionPtr)
			if (ConnectionPtr->State == adStateOpen)
				ConnectionPtr->Close();
		cout << "ConnectionPtr closed" << endl;
		::CoUninitialize();
		cout << "CoUninitialize ()" << endl;
	}
	catch (_com_error &e) {
		// Notify the user of errors if any.  
		_bstr_t bstrSource(e.Source());
		_bstr_t bstrDescription(e.Description());

		PrintProviderError();

		printf("Source : %s\n", (LPCSTR)bstrSource);
		printf("Description : %s\n", (LPCSTR)bstrDescription);
		printf("FAILED TO CLOSE CONNECTION");
	}
	catch (...) {
		printf("unknown exception : %s\n", "Close Connection");
	}
	connectionOpened = false;
}


void Database::PrintProviderError()
{
	// Print Provider Errors from Connection object.  
	// pErr is a record object in the Connection's Error collection.  
	ErrorPtr pErr = NULL;
	long nCount = 0;
	long i = 0;

	if ((ConnectionPtr->Errors->Count) > 0) {
		nCount = ConnectionPtr->Errors->Count;
		// Collection ranges from 0 to nCount -1.  
		for (i = 0; i < nCount; i++) {
			pErr = ConnectionPtr->Errors->GetItem(i);
			printf("Error number: %x\t%s\n", pErr->Number, (LPCSTR)pErr->Description);
		}
	}

}
string Database::GetTimeDate(time_point<system_clock> time) {
	stringstream ss;
	ss << time;
	return ss.str();
}

string Database::ToHexString(const unsigned int i)
{
	stringstream s;
	s << "0x" << hex << i;
	return s.str();
}

string Database::ToColor(const unsigned int i)
{
	string output;
	switch (i) {
	case 0xFF0000: output = "Red"; break; //red
	case 0x0000FF: output = "Blue"; break;//blue 
	}
	return output;
}


