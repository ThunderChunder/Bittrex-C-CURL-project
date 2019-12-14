//This project was developed by Brian Valenzi.
#include "Project.h"

int main(int argc, char **argv)
{	
	//Store all retrieved data into struct
//	coinData ethData;
//	coinData powrData;
//	coinData vtcData;
//	coinData merData;
//	coinData arkData;

	char URLFile[] = "URLs.txt";

	int lineCount = fCountLines(URLFile);

	coinData coinArr[lineCount];
	char *URLarr[lineCount];
	
	std::fstream inStream;
	inStream.open(URLFile);
	if(!inStream)
	{
		std::cerr<<"Unable to open." << URLFile <<'\n';
	}
	
	int i = 0;
	char temp[100];
	while(i<lineCount)
	{
		inStream>>temp;
		URLarr[i] = temp;
		coinArr[i].setName(URLarr[i]);
		setUpCurl(coinArr[i], URLarr[i]);
		coinArr[i].deleteMemory();
		i++;	
	}
	inStream.close();
	//setUpCurl(coinArr[i-1], URLarr[i-1]);
	storeDataPhase(lineCount, coinArr, URLarr);
	//Sets up handle for HTTP/S request and stores to struct.(struct, request URL)

	cleanUpCurl(ehandle);
	return 0;
}
