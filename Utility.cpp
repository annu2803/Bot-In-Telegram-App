#include "Utility.h"

char * GetCurrentTime()
{
        time_t clk = time(NULL);
        return ctime(&clk);
}

bool IsFileExists(const char* fname)
{
        struct stat64 buff64;
        int res = stat64(fname, &buff64);
        if (-1 != res)
                return true;

        return false;
}

bool CreateFile(const std::string& fname)
{
        int res = open(fname.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0644);
        if (-1 == res && errno != EEXIST)
                return false;

        return true;
}

void WriteToFile(const std::string& fname,const std::string& str)
{
        std::ofstream file_id;
        file_id.open(fname,std::ios_base::app);
        if(!file_id.is_open())
        {
                std::cout<<"file open failed "<<fname<<std::endl;
                return;
        }

        file_id<<str;
        file_id.close();
}

void ReadFile(const std::string& fname,std::string& retStr)
{
        std::ifstream file(fname);
        if(!file.is_open())
        {
                std::cout<<"file read failed "<<fname<<std::endl;
                return;
        }

        std::string str((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

        retStr = str;
        file.close();
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void SendCurlData(const std::string &url,std::string &response)
{
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                res = curl_easy_perform(curl);
                curl_easy_cleanup(curl);
        }
}

bool ParseString(Json::Value &root,const std::string &str)
{
	Json::Reader reader;
	return reader.parse(str,root);
}
bool ParseFile(Json::Value &root,const std::string &file)
{
	std::string str;
	ReadFile(file,str);
	str.insert(0,1,'[');
        str.append("]");

	Json::Reader reader;
	return reader.parse(str,root);
}

std::string UrlEncode(const std::string &s)
{
    static const char lookup[]= "0123456789abcdef";
    std::stringstream e;
    for(int i=0, ix=s.length(); i<ix; i++)
    {
        const char& c = s[i];
        if ( (48 <= c && c <= 57) ||//0-9
             (65 <= c && c <= 90) ||//abc...xyz
             (97 <= c && c <= 122) || //ABC...XYZ
             (c=='-' || c=='_' || c=='.' || c=='~')
        )
        {
            e << c;
        }
        else
        {
            e << '%';
            e << lookup[ (c&0xF0)>>4 ];
            e << lookup[ (c&0x0F) ];
        }
    }
    return e.str();
}

tm * GetLocalTime()
{
	const time_t tt = (time_t)time(NULL);
	return localtime(&tt);
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}
