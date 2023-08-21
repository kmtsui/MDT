#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::ifstream;
using std::map;
using std::string;
using std::stringstream;
using std::vector;

struct Value_t {
		int i;
		float f;
		string s;
};

class Configuration {
	public:

		static Configuration* GetInstance();
		void Finalize();

		void ReadParameter(string);
		void PrintParameters();
		void AddValue(string, string);
		template<typename T> T GetValue(string);
		template<typename T> void GetValue(string, T&);

		string ParseMDTROOT(string);

	protected:

		Configuration();

	private:

		static Configuration* fTheInstance;

		bool IsAvailable(string&, bool forceexit = true);
		map<string, Value_t> fMpValue;
};
