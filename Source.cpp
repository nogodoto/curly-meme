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
double CB[DICT_SIZE] = { 0.2, 0.6, 1.0}; //Cumulative probabilities set higher
int S[DICT_SIZE] = { 2, 1, 0 }; //Source file
int S_EOF = 0; //End of file symbol for S
bitset<CODE_SIZE> B;

//void enc1_intervLoop(Range r, int sPos, string soFar)
//{
//	double range = r._high - r._low;
//	for (int i = 0; i < DICT_SIZE; i++)
//	{
//		if (D[i] == S[sPos])
//		{
//			r = Range(r._low + range*cumP[i], r._low + range*cumP[i+1]);
//			soFar = soFar + to_string(S[sPos]) + " ";
//			cout << r._low << ", " << r._high << endl << soFar << endl;
//			enc1_intervLoop(r, sPos + 1, soFar);
//
//		}
//	}
//}

//void enc1_interv(Range r, int sPos, string soFar)
//{
//	if (sPos > 4)
//	{
//		interval = Range(r._low, r._high);
//		return;
//	}
//	long double range = r._high - r._low;
//	
//	r = Range(r._low + range*cumP[S[sPos]], r._low + range*cumP[S[sPos] + 1]);
//	soFar = soFar + to_string(S[sPos]);
//	/*r.setLow(r._low + range*cumP[S[sPos]]);
//	r.setHigh(r._low + range*cumP[S[sPos] + 1]);*/
//	cout << setprecision(9) << "r: " << r._low << ", " << r._high << endl << soFar << endl;
//
//	enc1_interv(r, sPos + 1, soFar);
//}

//void enc1_bin(Range AB, Range binInterv, string binCode)
//{
//	if (AB._high > binInterv._high && AB._low <= binInterv._low)
//	{
//		return;
//	}
//	long double mid = (binInterv._high - binInterv._low) / 2 + binInterv._low;
//	if (AB._low >= mid)
//	{
//		binInterv = Range(mid, binInterv._high);
//		binCode = binCode + "1";
//	}
//	else if (AB._high < mid)
//	{
//		binInterv = Range(binInterv._low, mid);
//		binCode = binCode + "0";
//	}
//	else if (AB._low <= mid && AB._high > mid)
//	{
//
//	}
//
//	cout << "binInterv: " << binInterv._low << ", " << binInterv._high << endl << "binCode: " << binCode << endl;
//	enc1_bin(AB, binInterv, binCode);
//}

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
		b = a + w * CB[S[i]];
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