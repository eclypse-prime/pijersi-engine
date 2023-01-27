#include <algorithm>
#include <vector>
#include <string>

#include <utils.hpp>

using std::string;
using std::vector;

namespace PijersiEngine::Utils
{
    vector<string> split(string str, string token){
        vector<string>result;
        while(str.size()){
            size_t index = str.find(token);
            if(index!=string::npos){
                result.push_back(str.substr(0,index));
                str = str.substr(index+token.size());
                if(str.size()==0)result.push_back(str);
            }else{
                result.push_back(str);
                str = "";
            }
        }
        return result;
    }

    string strip(string str)
    {
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.cend());
        return str;
    }
}