#ifndef DIARY_H
#define DIARY_H

#include "Utility.h"

#define BOT_CFG_FILE "botconfig.cfg"

std::string g_Weekday[7]= {"Sunday",
				"Monday",
				"Tuesday",
				"Wednesday",
				"Thursday",
				"Friday",
				"Saturday"};
struct BotConfig 
{
	std::string botID;
	std::string botToken;
	std::string msgID;
	std::string name;
};

class Diary
{
	public:
		Diary();
		~Diary();
		void Init();

	private:
		BotConfig *m_botCfg;
		std::string m_botData;
		std::string m_data;
		unsigned long long m_offset;
		std::string m_senderID;
		void ReadBotConfig();
		bool ReadData();
		void ProcessCommand();
		void ProcessData();
		void SendDailyReport();
		void SendMonthlyReport();
		void SendReport(int date = 0);
		void SetReminder();
		std::string ExtractInfoFromJson(const Json::Value &root,int day_);
		void SendToBot(std::string data);
		void SendFile(const std::string &file);

};

#endif
