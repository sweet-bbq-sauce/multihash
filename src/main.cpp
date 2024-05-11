#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <unordered_set>

#include <algorithm.hpp>
#include <output.hpp>


// Uppercase hash output flag
bool uppercase = false;

// Output format (plain, json or xml)
OutputFormat format = OutputFormat::PLAIN;

// Program args as std::vector
std::vector<std::string> args;

// Input file path
std::filesystem::path filepath;

// Used algorithms names array
std::vector<std::string> algorithms;

// Used algorithms objects
std::vector<Algorithm*> processors;


// Print version info
void printVersion(){

    std::cerr << "multihash v0.1" << std::endl << "Copyright (C) 2024 by Wiktor Sołtys (sweet-bbq-sauce)" << std::endl;

}



// Print help info
void printHelp(){

    std::cerr << "Usage:" << std::endl;
    std::cerr << "multihash [INPUT FILE] [ALGORITHMS] [OPTIONS]" << std::endl;

    std::cerr << std::endl;

    std::cerr << "Description:" << std::endl;
    std::cerr << "coś tam" << std::endl;

    std::cerr << std::endl;

    std::cerr << "Options:" << std::endl;
    std::cerr << "\t-a:<name>\tAdd hash algorithm" << std::endl;
    std::cerr << "\t-U\t\tOutput hash is uppercased" << std::endl;
    std::cerr << "\t-JSON\t\tFormat output as JSON" << std::endl;
    std::cerr << "\t-XML\t\tFormat output as XML" << std::endl;
    std::cerr << std::endl;
    std::cerr << "\t-h,--help\tPrint this information" << std::endl;
    std::cerr << "\t-v,--version\tPrint version" << std::endl;

    std::cerr << std::endl;

    std::cerr << "Algorithms:" << std::endl;

    std::unordered_set<std::string> methods;

    for(uint16_t id = 0; id < UINT16_MAX; id++){

        const EVP_MD* digest = EVP_get_digestbynid(id);

        if(digest != nullptr) methods.insert(EVP_MD_name(digest));

    }

    for(const std::string& method : methods) std::cerr << method << std::endl;
    
    std::cerr << std::endl;

    printVersion();

}



// On error. Print message end exit. In addition, it can print help if the error is due to ignorance of the program usage.
void dump(const std::string& message, bool withHelp){

    // Print message in red color
    #ifdef _WIN32

        std::cerr << "\x1b[31m" << message << "\x1b[0m" << std::endl;

    #else

        std::cerr << "\033[31m" << message << "\033[0m" << std::endl;

    #endif

    if(withHelp) printHelp();

    exit(EXIT_FAILURE);

}



int main(int argn, char* argv[]){


    // Put arguments to std::vector
    for(int i = 0; i < argn; i++) args.push_back(argv[i]);



    // Check if there is a "-h" or "--help"
    if(std::find(args.begin(), args.end(), "-h") != args.end()){
        printHelp();
        return EXIT_SUCCESS;
    }

    if(std::find(args.begin(), args.end(), "--help") != args.end()){
        printHelp();
        return EXIT_SUCCESS;
    }



    // Check if there is a "-v" or "--version"
    if(std::find(args.begin(), args.end(), "-v") != args.end()){
        printVersion();
        return EXIT_SUCCESS;
    }

    if(std::find(args.begin(), args.end(), "--version") != args.end()){
        printVersion();
        return EXIT_SUCCESS;
    }



    // Check if is there minimum 3 arguments (exec, input file and at least one algorithm)
    if(args.size() < 3) dump("Not enough arguments.", true);



    // Check if input is valid file path
    filepath = args[1];
    if(!std::filesystem::exists(filepath) || !std::filesystem::is_regular_file(filepath)) dump(filepath.string() + " not exists or it is not regular file.", false);



    // Check if there is a "-U" flag
    if(std::find(args.begin(), args.end(), "-U") != args.end()) uppercase = true;



    // Check if there is a "-JSON" flag
    if(std::find(args.begin(), args.end(), "-JSON") != args.end()) format = OutputFormat::JSON;



    // Check if there is a "-XML" flag
    if(std::find(args.begin(), args.end(), "-XML") != args.end()) format = OutputFormat::XML;



    // Get algorithms
    std::copy_if(args.begin(), args.end(), std::back_inserter(algorithms), [](const std::string& option){

        // Algorithm option must look like that: -a:<name>
        return option.size() > 3 && option.substr(0, 3) == "-a:";

    });

    for(std::string& algorithm : algorithms) algorithm.erase(0, 3);



    // Try to init all hash algorithms
    try{

        for(const std::string& name : algorithms) processors.push_back(new Algorithm(name));

    }
    catch(const std::exception& e){

        dump(e.what(), false);

    }



    // Try to open file
    std::ifstream input(filepath, std::ios::binary);
    if(input.bad()) dump(std::string("Can't open ") + filepath.string(), false);



    char buffer[1024*1024];


    while(!input.eof()){

        input.read(buffer, sizeof(buffer));

        for(Algorithm* current : processors) current->update(buffer, input.gcount());

    }

    input.close();

    std::map<std::string, std::string> output;

    for(int i = 0; i < algorithms.size(); i++){

        std::vector<uint8_t> digest;

        processors[i]->finish(digest);

        delete processors[i];

        output[algorithms[i]] = binToHex(digest, uppercase);

    }

    switch(format){

        case OutputFormat::PLAIN:
            outputPlain(output);
            break;

        case OutputFormat::JSON:
            outputJSON(output);
            break;

        case OutputFormat::XML:
            outputXML(output);
            break;

    }

    return EXIT_SUCCESS;

}