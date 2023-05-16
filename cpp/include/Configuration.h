#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

using std::cout;
using std::endl;
using std::vector;
using std::stringstream;
using std::string;
using std::ifstream;
using std::map;


struct Value_t
{
    int i;
    float f;
    string s;
};

class Configuration
{
    public :
        static Configuration* GetInstance();
        void Finalize();

        void ReadParameter(string);
        void PrintParameters();
		void AddValue(string, string);
        template <typename T> T GetValue(string);
        template <typename T> void GetValue(string, T&);

        string ParseMDTROOT(string);

    protected :
        Configuration();

    private :
        static Configuration *fTheInstance;

        bool IsAvailable(string&,bool forceexit=true);
        map<string, Value_t> fMpValue; 
};
