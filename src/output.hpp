#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <cinttypes>
#include <iomanip>



enum class OutputFormat {

    PLAIN,
    JSON,
    XML

};



void outputPlain(const std::map<std::string, std::string>& output){

    for(const auto& hash : output) std::cout << hash.first << ":\t" << hash.second << std::endl;

}



void outputJSON(const std::map<std::string, std::string>& output){

    auto it = output.begin();
    auto last = output.end();
    --last;

    std::cout << "[";

    for(; it != output.end(); ++it){

        std::cout << "{\"name\":\"" << it->first << "\",\"digest\":\"" << it->second << "\"}";

        if(it != last) std::cout << ",";

    }

    std::cout << "]" << std::flush;

}



void outputXML(const std::map<std::string, std::string>& output){

    std::cout << "<hashes>";

    for(const auto& hash : output){

        std::cout << "<hash algorithm=\"" << hash.first << "\">" << hash.second << "</hash>";

    }

    std::cout << "</hashes>" << std::flush;

}



std::string binToHex(const std::vector<uint8_t>& data, bool uppercase){

    std::ostringstream oss;

    oss << std::hex << std::setfill('0');

    if(uppercase) oss << std::uppercase;

    for(size_t i = 0; i < data.size(); i++) oss << std::setw(2) << static_cast<int>(data[i]);

    return oss.str();

}