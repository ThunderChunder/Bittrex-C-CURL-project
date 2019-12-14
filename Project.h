//This project was developed by Brian Valenzi.
#ifndef projHead
#define projHead

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <curl/curl.h>
#include <cstring>

//*******************PROTOTYPES****************************
struct coinData;
size_t writeData(void*, size_t, size_t, void*);
void setUpCurl(coinData &, char*);
//********************GLOBAL VARIABLES**********************
//Sets up libcurl environment. Call only once and must be global.
CURLcode curl = curl_global_init(CURL_GLOBAL_ALL);
//All options stored in the ehandle
CURL *ehandle = curl_easy_init();

//********************COIN STRUCT DEFINITIONS/FUNCTIONS***********************************
struct coinData
{
	char *memory;
	size_t size;
	char *name;
	coinData();
	~coinData();
	void setName(char*);
	void printName();
	void storeData();
	void deleteMemory();
};
struct evaluateEngine
{
		
};
coinData::coinData()
{
	memory = NULL;
	size = 0;
	name = new char[10];
	name[0] = '\0';	
}
coinData::~coinData()
{
	delete [] memory;
	delete [] name;	
}
void coinData::setName(char *url)
{	
	for(int i = 0; i < strlen(url); i++)
	{
		if(url[i] == '-')
		{
			strcpy(name, &url[i+1]); 
			break;
		}
	}
	printName();
}
void coinData::printName()
{
	std::cout<<name <<'\n';	
}
void coinData::storeData()
{
	char temp[20] = {'\0'};
	strcat(temp, "Marketdata/");
	strcat(temp, name);
	strcat(temp, ".txt");
	
	std::fstream outStream;
	outStream.open(temp, std::ios::app);
	if(!outStream)
	{
		std::cerr<<"Unable to open " << temp;	
	}
	int count = 0;
	for(int i = 0; i < strlen(memory); i++)
	{
		if(memory[i] == ':')
		{
			count++;
		}
		if(count == 4 &&  memory[i] != ':')
		{
			if(memory[i] == ',')
			{
				break;	
			}
			outStream<<memory[i];		
		}
	}
	outStream << '\n';
	outStream.close();
}
void coinData::deleteMemory()
{
	delete [] memory;
	delete [] name;	
}
//********************PROJECT FUNCTIONS***********************************
int fCountLines(char *fileName)
{
	std::fstream iStream;
	iStream.open(fileName);
	if(!iStream)
	{
		std::cerr << "File " << fileName << " unable to be opened.";
		exit(1);
	}
	char temp[100];
	int lineCount= 0;
	while(iStream)
	{
		iStream.getline(temp, 100);
		lineCount++;
	}
	iStream.close();
	return (lineCount-1);	
}
void storeDataPhase(int lineCount, coinData *coinArr, char URLarr[])
{
	time_t timer;
	time_t timeNow = time(0);
	time(&timer);
	timer /= 60;
	timer %= 10;
	int k;
	bool flow = true;
	while(true)
	{
		if(timer % 15 == 10)
		{
			flow = true;
		}
		time(&timer);
		timer /= 60;
		if(timer % 15 == 0 && flow == true)
		{
			for(k = 0; k < lineCount; k++)
			{
				coinArr[k].storeData();
				coinArr[k].deleteMemory();
				setUpCurl(coinArr[k], URLarr[k]);
			}
			std::cout<<"Data Retrieval complete."<<'\n';
			flow = false;
		}
	}
}
//*********************CURL FUNCTIONS*************************************
void setUpCurl(coinData &cData, char *URL)
{
	//Used to set URL to get.
	curl_easy_setopt(ehandle, CURLOPT_URL, URL);
	
	//Dont verify peer cert
	//FIX LATER BY ACQUIRING CERT FOR BITTREX
	curl_easy_setopt(ehandle, CURLOPT_SSL_VERIFYPEER, FALSE);
	
	//Data sent to this function specified
	curl_easy_setopt(ehandle, CURLOPT_WRITEFUNCTION, writeData);
	
	//pass coinData struct to write to
	curl_easy_setopt(ehandle, CURLOPT_WRITEDATA, (void *)&cData);
	
	//Performs transfer based on options set with ...setopt();
	CURLcode response = curl_easy_perform(ehandle);
	
	//Error check
	if(response != CURLE_OK)
	{
		std::cerr << "Error code: " << curl_easy_strerror(response) << std::endl;	
	}
}
size_t writeData(void *buffer, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size*nmemb;
	struct coinData *mem = (struct coinData *)userp;
	mem->memory = (char *)malloc(mem->size + realsize+1);
	//mem->memory = new char(mem->size + realsize+1);
	if(mem->memory == NULL)
	{
		std::cout << "Not enough memory allocated.\n";
		exit(0);
	}
	//memcpy(&(mem->memory[mem->size]), buffer, realsize);
	strcpy((char *)(mem->memory), (char *)buffer);
	mem->size  += realsize;
	
	std::cout<<mem->memory;
	
	return realsize;
}
void cleanUpCurl(CURL *ehandle)
{
	//Clean up curl handle
	curl_easy_cleanup(ehandle);
	
	//Clean up libcurl environment
	curl_global_cleanup();	
}
#endif
