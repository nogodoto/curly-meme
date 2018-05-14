#include <stdio.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <bitset>
#include <math.h>
#include <time.h>

using namespace std;

// Testing Model
const int DICT_SIZE = 3;
const int CODE_SIZE = 16;
int D[DICT_SIZE] = { 0, 1, 2 }; // Dictionary of symbols
double P[DICT_SIZE] = { 0.2, 0.4, 0.4 }; //Probability set
double CA[DICT_SIZE] = { 0, 0.2, 0.6 }; //Cumulative probabilities set lower
double CB[DICT_SIZE] = { 0.2, 0.6, 1.0 }; //Cumulative probabilities set higher
int S[DICT_SIZE] = { 2, 1, 0 }; //Source file
int S_EOF = 0; //End of file symbol for S
bitset<CODE_SIZE> B;
int PA[3];  //Adaptive probability
int adaptiveStats(int bit, int i)//determines forecasted probability for bit positions
{
	int scale_factor = 4096;
	int probFor1 = scale_factor/2.0;
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

//Expands binary fraction for use in decoding
long double expandBitset()
{
	long double exp = 0.0;
	for (int i = CODE_SIZE - 1; i >= 0; i--)
	{
		if (B[i] == 1)
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
	for (int i = 0; i < DICT_SIZE; i++)
	{
		long double w = b - a; //width
		//int adaptiveStats(int bit, int i);
		b = a + w * CB[S[i]]; //PA[S[i]]
		a = a + w * CA[S[i]];
	}
	//cout << a << ", " << b << endl;

	//Part 2: emit binary encoding of [a, b)
	int s = 0;
	int binPos = CODE_SIZE; //Tracks input to bitset B
	string binCode = "";
	while (b < 0.5 || a > 0.5)
	{
		binPos--;
		if (b <0.5) //Case 0
		{
			binCode = binCode + "0"; //Emit 0
									 //cout << "Case 0 : " << binCode << endl;
			a = 2 * a;
			b = 2 * b;
		}
		else if (a > 0.5) //Case 1
		{
			binCode = binCode + "1"; //Emit 1
			B.set(binPos);
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
	binPos--;
	if (a <= 0.25) //Case Q (take first quarter)
	{
		binCode = binCode + "0"; //Emit 0
		for (int i = 0; i < s; i++) //Emit s 1s
		{
			binCode = binCode + "1";
			B.set(binPos);
			binPos--;
		}
		//cout << "Case Q : " << binCode << endl;
	}
	else //Case 3Q (take third quarter)
	{
		binCode = binCode + "1"; //Emit 1
		B.set(binPos);
		for (int i = 0; i < s; i++) //Emit s 0s
		{
			binCode = binCode + "0";
		}
		//cout << "Case 3Q : " << binCode << endl;
	}

	return binCode;
}

//Assuming infinite precision, decodes binary sequence S, emits binary code as string
string decode_inf()
{
	long double a = 0.0, b = 1.0, z = expandBitset();
	string symSeq = "";
	while (true)
	{
		//Part 1: try ranges to find z within [a, b) 
		for (int i = 0; i < DICT_SIZE; i++)
		{
			long double w = b - a; //width
			long double bCand = a + w * CB[i];
			long double aCand = a + w * CA[i];
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

int main()
{
	/*Code for checking runtimes
	clock_t t1, t2;
	t1 = clock();
	t2 = clock();
	float diff1(((float)t2 - (float)t1) / CLOCKS_PER_SEC);
	cout << setprecision(9) << "Time for Encoding: " << diff1 << " seconds" << endl << endl;*/

	cout << "Encoded: " << encode_inf() << endl;




	cout << "Bitset: " << B << endl;

	cout << "Decoded: " << decode_inf() << endl;


	system("PAUSE");
	return 0;

}