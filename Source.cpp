#include <stdio.h>
#include <iostream>
#include <string>
#include<fstream>

#pragma pack(2)

using namespace std;

const int DICT_SIZE = 5;

struct Range {
	double _low;
	double _high;
	Range(double l, double h) {
		_low = l;
		_high = h;
	}
};
struct BmpSignature
{
	unsigned char data[2];
};

struct BmpHeader
{
	BmpSignature signature;
	unsigned int fileSize;
	unsigned short reserved1;
	unsigned short reserved2;
	unsigned int dataOffset;
};

int D[DICT_SIZE] = { 0, 1, 2, 3, 4 }; // Dictionary holding symbols
double P[DICT_SIZE] = { 0.1, 0.2, 0.4, 0.2, 0.1 }; //Probability set
double cumP[DICT_SIZE + 1] = { 0, 0.1, 0.3, 0.7, 0.9, 1.0 }; //Cumulative probabilities set
int S[DICT_SIZE] = { 3, 4, 1, 0, 2 }; //Source file
Range interval(0.0, 1.0); //Initial starting interval

void enc1_interv(Range r, int sPos, string soFar)
{
	double range = r._high - r._low;
	for (int i = 0; i < DICT_SIZE; i++)
	{
		if (D[i] == S[sPos])
		{
			r = Range(r._low + range*cumP[i], r._low + range*cumP[i + 1]);
			soFar = soFar + to_string(S[sPos]) + " ";
			cout << r._low << ", " << r._high << endl << soFar << endl;
			enc1_interv(r, sPos + 1, soFar);

		}
		//cumFreq += (1 - cumFreq) * P[i];
	}
}

void enc1_bin()
{

}


int main()
{


	BmpHeader header;
	string FILE = "C:/Tyler/source/repos/ArithmeticEncoding/image.bmp";  //enter filepath of image here
	ifstream inFile;
	inFile.open(FILE);
	if (inFile.is_open())
	{
		void inputHeaderhelper(ifstream &inFile, BmpHeader &header);

	}
	cout <<"File size prior to compression is"<< PrintSIZE(header)<< "bytes" << endl;

	string soFar = "";
	enc1_interv(interval, 0, soFar);




	system("PAUSE");
	return 0;
}
void inputHeaderhelper(ifstream &inFile, BmpHeader &header)
{
	if (!inFile)
		return;

	inFile.seekg(0, ios::beg);
	inFile.read((char*)&header, sizeof(header));


}
int PrintSIZE(BmpHeader header)
{
	cout << header.fileSize;

}