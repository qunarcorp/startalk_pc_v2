#include "Md5.h"
#include <iostream> 

using namespace std;

void PrintMD5(const string &str, MD5 &md5) {
	cout << "MD5(\"" << str << "\") = " << md5.toString() << endl;
}

string FileDigest(const string &file) {

	ifstream in(file.c_str(), ios::binary);
	if (!in)
		return "";

	MD5 md5;
	std::streamsize length;
	char buffer[1024];
	while (!in.eof()) {
		in.read(buffer, 1024);
		length = in.gcount();
		if (length > 0)
			md5.update(buffer, length);
	}
	in.close();
	return md5.toString();
}

int test() 
{

	cout << MD5("abc").toString() << endl;
	ifstream f("D:\\test.txt");
	cout << MD5(f).toString() << endl;
	f = ifstream("D:\\test.exe", ios::binary);
	cout << MD5(f).toString() << endl;
	cout << FileDigest("D:\\test.exe") << endl;

	MD5 md5;
	md5.update("");
	PrintMD5("", md5);

	md5.update("a");
	PrintMD5("a", md5);

	md5.update("bc");
	PrintMD5("abc", md5);

	md5.update("defghijklmnopqrstuvwxyz");
	PrintMD5("abcdefghijklmnopqrstuvwxyz", md5);

	md5.reset();
	md5.update("message digest");
	PrintMD5("message digest", md5);

	md5.reset();
	ifstream ff("D:\\test.txt");
	md5.update(ff);
	PrintMD5("D:\\test.txt", md5);
	return 0;
}
