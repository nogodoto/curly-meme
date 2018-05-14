#include <stdio.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <bitset>
#include <vector>
#include <math.h>
#include <time.h>

using namespace std;

//Testing model basics
const int DICT_SIZE = 3;
const int IN_SIZE = 5;
const int CODE_SIZE = 32;
int D[DICT_SIZE] = { 0, 1, 2 }; // Dictionary of symbols
double P[DICT_SIZE] = { 0.2, 0.4, 0.4 }; //Probability set
int S[IN_SIZE] = { 2, 2, 1, 1, 0 }; //Source file
int S_EOF = 0; //End of file symbol for S
int PA[3];  //Adaptive probability

//Testing Model for infinite precision
double CAI[DICT_SIZE] = { 0, 0.2, 0.6 }; //Cumulative probabilities set lower
double CBI[DICT_SIZE] = { 0.2, 0.6, 1.0}; //Cumulative probabilities set higher
bitset<CODE_SIZE> BI;

//Testing Model for finite precision
int PREC = 16; //Number of bits used to represent numbers
int WHOLE = pow(2, PREC);
int HALF = WHOLE / 2;
int QUARTER = WHOLE / 4;
int R = 10;
int P_R[DICT_SIZE] = { 2, 4, 4 }; //Probability set
int CAF[DICT_SIZE] = { 0, 2, 6 }; //Cumulative probabilities set lower
int CBF[DICT_SIZE] = { 2, 6, 10 }; //Cumulative probabilities set higher
//bitset<CODE_SIZE> BF;
vector<bool> BFv;

//Expands binary fraction for use in decoding
long double expandBitset_inf()
{
	long double exp = 0.0;
	for (int i = CODE_SIZE - 1; i >= 0; i--)
	{
		if (BI[i] == 1)
		{
			exp += 1 / (pow(2, CODE_SIZE - i));
		}
	}
	return exp;
}

//Assuming infinite precision, encodes symbol sequence S, emits binary code as string
string encode_inf()
{
	long double a = 0.0, b = 1.0;
	//Part 1: find range [a, b) 
	for (int i = 0; i < IN_SIZE; i++)
	{
		long double w = b - a; //width
		b = a + w * CBI[S[i]];
		a = a + w * CAI[S[i]];
	}
	//cout << a << ", " << b << endl;

	//Part 2: emit binary encoding of [a, b)
	int s = 0;
	int binPos = CODE_SIZE - 1; //Tracks input to bitset B
	string binCode = "";
	while (b < 0.5 || a > 0.5)
	{
		if (b <0.5) //Case 0
		{
			binCode = binCode + "0"; //Emit 0
			binPos--;
			//cout << "Case 0 : " << binCode << endl;
			a = 2 * a;
			b = 2 * b;
		}
		else if (a > 0.5) //Case 1
		{
			binCode = binCode + "1"; //Emit 1
			BI.set(binPos);
			binPos--;
			//cout << "Case 1 : " << binCode << endl;
			a = 2 * (a - 0.5);
			b = 2 * (b - 0.5);
		}
	}

	//Case S (take middle two quarters)
	while (a > 0.25 && b < 0.75)
	{
		s++;
		a = 2 * (a - 0.25);
		b = 2 * (b - 0.25);
	}

	s++;
	if (a <= 0.25) //Case Q (take first quarter)
	{
		binCode = binCode + "0"; //Emit 0
		binPos--;
		for (int i = 0; i < s; i++) //Emit s 1s
		{
			binCode = binCode + "1";
			BI.set(binPos);
			binPos--;
		}
		//cout << "Case Q : " << binCode << endl;
	}
	else //Case 3Q (take third quarter)
	{
		binCode = binCode + "1"; //Emit 1
		BI.set(binPos);
		binPos--;
		for (int i = 0; i < s; i++) //Emit s 0s
		{
			binCode = binCode + "0";
			binPos--;
		}
		//cout << "Case 3Q : " << binCode << endl;
	}

	return binCode;
}

//Assuming infinite precision, decodes binary sequence S, emits binary code as string
string decode_inf()
{
	long double a = 0.0, b = 1.0, z = expandBitset_inf();
	string symSeq = "";
	while (true)
	{
		//Try ranges to find z within [a, b) 
		for (int i = 0; i < DICT_SIZE; i++)
		{
			long double w = b - a; //width
			long double bCand = a + w * CBI[i];
			long double aCand = a + w * CAI[i];
			if (aCand <= z && bCand > z) //Correct interval for symbol i
			{
				symSeq = symSeq + to_string(i); //Emit i
				a = aCand;
				b = bCand;
				if (i == S_EOF)
				{
					return symSeq;
				}
			}
		}
	}
}

//Expands binary fraction for use in decoding
unsigned long int expandBitset()
{
	unsigned long int exp = 0;
	for (int i = 0; i < BFv.size(); i++)
	{
		if (BFv[i] == 1)
		{
			exp += pow(2, PREC - i - 1);
		}
	}
	return exp;
}

//Rounds consistently and evenly
unsigned long int round(unsigned long int num, unsigned long int denom)
{
	double exact = (double)num / (double)denom;
	return ceil(exact);
}

//Assuming finite precision, encodes symbol sequence S, emits binary code as string
string encode()
{
	string binCode = "";
	unsigned long int a = 0, b = WHOLE;
	int s = 0;
	int binPos = CODE_SIZE - 1; //Tracks input to bitset

	for (int i = 0; i < IN_SIZE; i++)
	{
		unsigned long int w = b - a; //width
		b = a + round(w * CBF[S[i]], R);
		a = a + round(w * CAF[S[i]], R);

		while (b < HALF || a > HALF)
		{
			if (b < HALF) //Case 0
			{
				binCode = binCode + "0"; //Emit 0
				BFv.push_back(0);
				binPos--;
				for (int j = 0; j < s; j++) //Emit s 1s
				{
					binCode = binCode + "1";
					BFv.push_back(1);
					binPos--;
				}
				s = 0;
				a = 2 * a;
				b = 2 * b;
			}
			else if (a > HALF) //Case 1
			{
				binCode = binCode + "1"; //Emit 1
				BFv.push_back(1);
				binPos--;
				for (int j = 0; j < s; j++) //Emit s 0s
				{
					binCode = binCode + "0";
					BFv.push_back(0);
					binPos--;
				}
				s = 0;
				a = 2 * (a - HALF);
				b = 2 * (b - HALF);
			}
		}

		//Case S (take middle two quarters)
		while (a > QUARTER && b < QUARTER * 3)
		{
			s++;
			a = 2 * (a - QUARTER);
			b = 2 * (b - QUARTER);
		}
	}

	s++;
	if (a <= QUARTER) //Case Q (take first quarter)
	{
		binCode = binCode + "0"; //Emit 0
		BFv.push_back(0);
		binPos--;
		for (int i = 0; i < s; i++) //Emit s 1s
		{
			binCode = binCode + "1";
			BFv.push_back(1);
			binPos--;
		}
		//cout << "Case Q : " << binCode << endl;
	}
	else //Case 3Q (take third quarter)
	{
		binCode = binCode + "1"; //Emit 1
		BFv.push_back(1);
		binPos--;
		for (int i = 0; i < s; i++) //Emit s 0s
		{
			binCode = binCode + "0";
			BFv.push_back(0);
			binPos--;
		}
		//cout << "Case 3Q : " << binCode << endl;
	}
	
	return binCode;
}

//Assuming finite precision, decodes binary sequence S, emits binary code as string
string decode()
{
	unsigned long int a = 0, b = WHOLE, z = expandBitset();
	string symSeq = "";
	while (true)
	{
		//Try ranges to find z within [a, b) 
		for (int i = 0; i < DICT_SIZE; i++)
		{
			unsigned long int w = b - a; //width
			unsigned long int bCand = a + round(w * CBF[i], R);
			unsigned long int aCand = a + round(w * CAF[i], R);
			if (aCand <= z && bCand > z) //Correct interval for symbol i
			{
				symSeq = symSeq + to_string(i); //Emit i
				a = aCand;
				b = bCand;
				if (i == S_EOF)
				{
					return symSeq;
				}
				break;
			}
		}
		while (b < HALF || a > HALF)
		{
			if (b < HALF)
			{
				a = 2 * a;
				b = 2 * b;
				z = 2 * z;
			}
			else if (a > HALF)
			{
				a = 2 * (a - HALF);
				b = 2 * (b - HALF);
				z = 2 * (z - HALF);
			}
		}
		while (a > QUARTER && b < QUARTER * 3)
		{
			a = 2 * (a - QUARTER);
			b = 2 * (b - QUARTER);
			z = 2 * (z - QUARTER);
		}
	}
}

int adaptiveStats(int bit, int i)//determines forecasted probability for bit positions
{
	int scale_factor = 4096;
	int probFor1 = scale_factor / 2.0;
	if (bit == 0)
	{
		probFor1 = probFor1 >> 5;
	}
	else
	{
		probFor1 = (scale_factor - probFor1) >> 5;
	}

	PA[i] = probFor1;
}

int main()
{
	/*Code for checking runtimes
	clock_t t1, t2;
	t1 = clock();
	t2 = clock();
	float diff1(((float)t2 - (float)t1) / CLOCKS_PER_SEC);
	cout << setprecision(9) << "Time for Encoding: " << diff1 << " seconds" << endl << endl;*/

	cout << "Encoded (inf): \t" << encode_inf() << endl;
	cout << "Bitset: \t" << BI << endl;
	cout << "Decoded (inf): \t" << decode_inf() << endl << endl;

	cout << "Encoded (fin): \t" << encode() << endl;
	cout << "Bit vector: \t";
	for (int i = 0; i < BFv.size(); i++)
	{
		cout << BFv[i];
	}
	cout << endl;
	cout << "Decoded (fin): \t" << decode() << endl << endl;



	system("PAUSE");
	return 0;
}