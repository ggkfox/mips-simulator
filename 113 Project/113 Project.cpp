// 113 Project.cpp : Defines the entry point for the console application.
//Joshua Holland

#include"stdafx.h"
#include<stdlib.h>
#include<iostream>
#include<fstream>
#include<bitset>
#include<sstream>
#include<string>
#include<queue>
using namespace std;

queue<string> textInstruction;
queue<string> dateSegment;
struct memory {
	string address;
	string instruction;
};

memory textSegment[2048];
memory staticDataSegment[4096];
memory stackSegment[2048];
int registers[32];
int stackPointer = 0;
int pc = 4194304; //0x400000 in decimal
queue<string> instruction;
queue<string> dataSegment;

string hexToBinary(string hex);         //String -> String
string binaryToHex(string binary);		//String -> String
int binaryToDecimal(string binary);     //string -> Int
string decimalToBinary(int decimal);    //Int    -> String
string twosCompliment(int num);
void rFormat(string input);
void iFormat(string inupt);
void jFormat(string input);
void printRegister();

int main() {
	registers[13] = binaryToDecimal(hexToBinary("0x53fc"));
	registers[14] = binaryToDecimal(hexToBinary("0x6405"));
	registers[16] = binaryToDecimal(hexToBinary("0x10010000"));
	registers[24] = binaryToDecimal(hexToBinary("0x0"));
	registers[25] = binaryToDecimal(hexToBinary("0x0"));

	ifstream file("./hexfile.txt");
	string str;
	cout << "0x10010000" << "    " << binaryToHex(decimalToBinary(binaryToDecimal(hexToBinary("0x10010000")))) << endl;
	for (int i = 0; !file.eof(); i++) {
		getline(file, str);
		if (str == "DATA SEGMENT") break;
		textSegment[i].instruction = hexToBinary(str);
		textSegment[i].address = to_string((i*4)+0x400000);
		cout << textSegment[i].instruction << endl;
	}
	for (int i = 0; !file.eof(); i++) {
		getline(file, str);
		staticDataSegment[i].address = str.substr(2,8);
		textSegment[i].instruction = hexToBinary(str.substr(13,8));
	}

	for (bool stop = false; stop == false; pc += 4) {
		stop = true;
		for (int i = 0; textSegment[i].instruction != ""; i++) {
			if (binaryToDecimal(textSegment[i].address) == pc) {
				if (textSegment[i].instruction.substr(0, 6) == "000000") {
					rFormat(textSegment[i].instruction);
				}
				else if (textSegment[i].instruction.substr(0, 6) == "000010" || textSegment[i].instruction.substr(0, 6) == "000011") {
					jFormat(textSegment[i].instruction);
				}
				else iFormat(textSegment[i].instruction);
				stop = false;
				break;
			}
		}
	}
	//print registers
	for (int i = 0; i < 32; i++) {
		cout << i << " : " << binaryToHex(decimalToBinary(registers[i])) << endl;
	}
	//print text segment
	for (int i = 0; textSegment[i].instruction != ""; i++) {
		cout << textSegment[i].address << " : " << textSegment[i].instruction << endl;
	}
	//print data segment
	for (int i = 0; staticDataSegment[i].instruction != ""; i++) {
		cout << staticDataSegment[i].address << " : " << staticDataSegment[i].instruction << endl;
	}

	exit(0);
}

/*
*  __________  ___      ___ ____       ___    _______   ___________   _    ___________    ___        _   _________
*  | ________| | |      | | |   \      | |   / ______| |____   ____| | |  / _________ \  |   \      | | / ________|
*  | |         | |      | | | |\ \     | |  / /             | |      | | / /         \ \ | |\ \     | | | |
*  | |_______  | |      | | | | \ \    | | / /              | |      | | | |         | | | | \ \    | | | |_______
*  | ________| | |      | | | |  \ \   | | | |              | |      | | | |         | | | |  \ \   | | \_______  \
*  | |         | |      | | | |   \ \  | | | |              | |      | | | |         | | | |   \ \  | |         | |
*  | |         | |      | | | |    \ \ | | \ \              | |      | | | |         | | | |    \ \ | |         | |
*  | |         \ \______/ / | |     \ \| |  \ \______       | |      | |  \ \________/ / | |     \ \| |  _______/ /
*  |_|          \________/  |_|      \___|   \_______|      |_|      |_|   \__________/  |_|      \___| |________/
*
*/

string hexToBinary(string hexCode) {
	string binary;
	unsigned n;
	stringstream line;
	line << hex << hexCode;
	line >> n;
	bitset<32> s(n);
	binary = s.to_string();
	return binary;
}
string binaryToHex(string binary) {
	bitset<32> set(binary);
	stringstream toHex;
	toHex << hex << set.to_ulong();
	string hex = toHex.str();
	return hex;
}
int binaryToDecimal(string binary) {
	return stoi(binary, NULL, 2);
}
string decimalToBinary(int decimal) {
	string s = bitset<32>(decimal).to_string();
	return s;
}
string twosCompliment(int num) {
	return decimalToBinary((-num)-1);
}
void rFormat(string input) {
	int op = binaryToDecimal(input.substr(0, 6));
	int rs = registers[binaryToDecimal(input.substr(6, 5))];
	int rt = registers[binaryToDecimal(input.substr(11, 5))];
	int rd = binaryToDecimal(input.substr(16, 5));
	int shift = binaryToDecimal(input.substr(21, 5));
	int funct = binaryToDecimal(input.substr(26, 6));
	int result = 0;
	
	if (funct == 32) { //add
		result = rs + rt;
	}
	if (funct == 34) { //sub
		result = rs - rt;
	}
	else if (funct == 36) { //and
		result = rs & rt;
	}
	else if (funct == 37) { //or
		result = rs | rt;
	}
	else if (funct == 42) { //slt
		result == (rs < rt ? 1 : 0);
	}

	registers[rd] = result;
}
void iFormat(string input) {
	int op = binaryToDecimal(input.substr(0, 6));
	int rs = binaryToDecimal(input.substr(6, 5));
	int rt = binaryToDecimal(input.substr(11, 5));
	int immediate = binaryToDecimal(input.substr(16, 16));
	int result;

	if (op == 35) { //lw
		registers[rt] = immediate;
	}
	if (op == 43) { //sw
		stackSegment[stackPointer].instruction = immediate;
		stackSegment[stackPointer].address = 0x7fffefff+(stackPointer*4);
		stackPointer++;
	}
	else if (op == 4) { //beq
		if (rs == rt) {
			pc += (immediate * 4) -4;
		}
	}
	else if (op == 5) { //bne
		if (rs != rt) {
			pc += (immediate * 4) -4;
		}
	}
}
void jFormat(string input) {
	int op = binaryToDecimal(input.substr(0, 6));
	int address = binaryToDecimal(input.substr(6, 26));

	if (op == 2) { //j
		pc = address;
	}

}
void printRegister() {

}
