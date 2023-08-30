#include "Diary.h"

Diary::Diary()
{
	m_botCfg = new BotConfig;
	m_offset = 1;
	m_senderID = 1;
}

Diary::~Diary()
{
	delete m_botCfg;
	m_botCfg = nullptr;
}

int main()
{
	Diary myDiary;
	myDiary.Init();
	return 0;
}

void Diary::ReadBotConfig()
{
	Json::Value root;
	if(ParseFile(root,BOT_CFG_FILE)	== false)
	{
		cout<<"bot config file parse failed"<<endl;
		exit(0);
	}

	m_botCfg->botID = root[0]["BotID"].asString();
	m_botCfg->botToken = root[0]["BotToken"].asString();
	m_botCfg->msgID = root[0]["MessageID"].asString();
	m_botCfg->name = root[0]["Name"].asString();
}

void Diary::SendToBot(std::string data)
{
	data = ::UrlEncode(data);
	std::string url = "https://api.telegram.org/bot"+m_botCfg->botID+":"+m_botCfg->botToken+"/sendMessage?chat_id="+
		m_senderID+"&&text="+data;
	std::string response;
	::SendCurlData(url,response);
}

bool Diary::ReadData()
{
	std::string url = "https://api.telegram.org/bot"+m_botCfg->botID+":"+m_botCfg->botToken+"/getUpdates?offset="+to_string(m_offset)+"?limit=1";	
	Json::Value root;
	m_botData.clear();
	::SendCurlData(url,m_botData);
	m_data.clear();
	m_senderID = 1;
	std::string senderName = "",msg="";
	if(::ParseString(root,m_botData))
	{
		for (const Json::Value& text : root["result"])
		{
			m_offset=text["update_id"].asInt()+1;
			m_data = text["message"]["text"].asString();
			m_senderID = text["message"]["from"]["id"].asString();
			senderName = text["message"]["from"]["first_name"].asString();
			msg = text["message"]["text"].asString();
		}
	}

	if(m_senderID != m_botCfg->msgID)
	{
		SendToBot("Sorry you are not authorised");
		m_data.clear();
		m_senderID = m_botCfg->msgID;
		if(senderName != "")
		SendToBot("Got invalid access to bot from "+senderName+"\n\""+msg+"\"");
	}
	return (m_data != "");
}


void Diary::ProcessData()
{
	Json::Value event;		
	tm *local_time = GetLocalTime();
	std::string month = to_string(local_time->tm_mon+1);
	std::string year = to_string(1900 + local_time->tm_year);

	event["note"] = m_data;
	event["day"] = to_string(local_time->tm_mday);
	event["month"] = month;
	event["year"] = year;
	event["wday"] = g_Weekday[local_time->tm_wday];
	Json::FastWriter fastWriter;
	std::string data = "";
	std::string fileName =  "/opt/data/Event_"+m_botCfg->botID+"_"+year+month;
	if(IsFileExists(fileName.c_str()))
		data=",";

	data += fastWriter.write(event);
	WriteToFile(fileName,data);
}

std::string Diary::ExtractInfoFromJson(const Json::Value &root,int day_)
{
	std::string date[32]={""};
	std::string wday[32]={""};
	int count[32]={0};
	Json::FastWriter fastWriter;
	std::string str = fastWriter.write(root);
	bool today = false;
	if(day_)
		today = true;
	for (const Json::Value& text : root)
	{
		std::string dayStr = text["day"].asString();
		int day = stoi(dayStr);
		if(today)
		{
			if(day != day_)
				continue;
		}	
		if(count[day]++ == 0)
		{
			date[day] += "Day : ";
			date[day] += text["wday"].asString();
			date[day] += "\n";
		}
		date[day] += "# ";
		date[day] += text["note"].asString();
		date[day] += "\n";
	}
	std::string consolidated = "";
	tm *local_time = GetLocalTime();
	std::string month = to_string(local_time->tm_mon+1);
	std::string year = to_string(1900 + local_time->tm_year);
	consolidated += "Month: ";
	consolidated += month;
	consolidated += " Year: ";
	consolidated += year;
	consolidated += " ";
	for(int i = 1;i<32;i++)
	{
		if(date[i] != "")
		{
			consolidated += "\n";
			consolidated += "date: ";
			consolidated += to_string(i);
			consolidated += "\n";
			consolidated += date[i];
			consolidated += "\n";
		}
	}
	return consolidated;
}

void Diary::SendFile(const std::string &file)
{
	if(!IsFileExists(file.c_str()))
	{
		SendToBot(file+" does not exist\nSending failed");
		return;
	}

	std::string cmd = "curl -v -F \"chat_id="+m_botCfg->msgID+"\" -F document=@"+file+" https://api.telegram.org/bot"+m_botCfg->botID+
			":"+m_botCfg->botToken+"/sendDocument";
	system(cmd.c_str());
}

void Diary::SendReport(int date)
{
	tm *local_time = GetLocalTime();
	std::string month = to_string(local_time->tm_mon+1);
	if(!date)
		SendToBot("I am sending report for the month of "+month);
	else
		SendToBot("I am sending report for the day");

        std::string year = to_string(1900 + local_time->tm_year);
	std::string fileName = "/opt/data/Event_"+m_botCfg->botID+"_"+year+month;
	Json::Value root;
	if(!IsFileExists(fileName.c_str()) || !ParseFile(root,fileName))
	{
		SendToBot("file for "+year+" month "+month+" has issue\nCould not download");
		return;
	}

	std::string report = ExtractInfoFromJson(root,date);
	std::string dlFile = "/tmp/Events_"+m_botCfg->botID+"_"+year+month+".txt";
	unlink(dlFile.c_str());
	WriteToFile(dlFile,report);
	SendFile(dlFile);
}

void Diary::SendDailyReport()
{
	SendToBot("Invalid command");
	return;
}

void Diary::ProcessCommand()
{
	SendToBot("Please wait until your request is processed...");
	tm *local_time = GetLocalTime();
	if(m_data == "/month")
	{
		SendReport();
		return;
	}	
	if(m_data == "/day")
	{
		SendReport(local_time->tm_mday);
		return;
	}

	if(m_data == "/week")
	{
		SendToBot("Nandeesh has not implemented this still");
		return;
	}
	size_t pos = m_data.find("/day");
	if(pos != std::string::npos)
	{
		m_data.erase(0,pos+5);
		if(is_number(m_data))
		{
			int date = std::stoi(m_data);
			if(date <= local_time->tm_mday && (date != 0))
				SendReport(date);
			else
				SendToBot("Invalid date");
			return;
		}	
	}



	SendToBot(m_data+" is not a valid command");
}

void Diary::Init()
{
	ReadBotConfig();
	SendToBot("Hello "+m_botCfg->name+", \nI am your PA\nFeel free to share your day to day happenings");
	int prevMon = 0;
	while(1)
	{
		sleep(0);
		tm *local_time = GetLocalTime();
		if(prevMon != local_time->tm_mon)
		{
			SendToBot("Your Monthly Report for previous month");
			if(prevMon != 11)
				SendReport();
			else
				SendToBot("Sorry! Please download it manually");
		}
		prevMon = local_time->tm_mon;
		if(!ReadData())
			continue;
		
		if(m_data.find("/") != string::npos)
		{
			ProcessCommand();
			continue;
		}

		ProcessData();
		if(m_data != "")
			SendToBot(m_botCfg->name+", I have noted\n\""+m_data+"\"");
		else
			SendToBot(m_botCfg->name+", Sorry! I could not parse");
	}
}

