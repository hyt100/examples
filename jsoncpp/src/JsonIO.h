#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include "json/json.h"

class JsonIO
{
public:
    static bool LoadFromString(const std::string &content, Json::Value &root)
    {
        JSONCPP_STRING errs;
        Json::CharReaderBuilder rbuilder;
        std::unique_ptr<Json::CharReader> const jsonReader(rbuilder.newCharReader());
        bool ok = jsonReader->parse(content.c_str(), content.c_str() + content.length(), &root, &errs);
        if (!ok || !errs.empty()) {
            std::cout << "Load json error: " << errs << std::endl;
            return false;
        }
        return true;
    }

    static bool SaveToString(std::string &content, Json::Value &root)
    {
        std::ostringstream os;
        Json::StreamWriterBuilder wbuilder;
        std::unique_ptr<Json::StreamWriter> jsonWriter(wbuilder.newStreamWriter());
        jsonWriter->write(root, &os);
        content = os.str();
        return true;
    }

    static bool Load(const std::string &filename, Json::Value &root)
    {
        std::ifstream os(filename);
        if (!os.is_open()) {
            std::cout << "open " << filename << " failed." << std::endl;
            return false;
        }

        Json::CharReaderBuilder rbuilder;
        std::string errs;
        bool ok = Json::parseFromStream(rbuilder, os, &root, &errs);
        if (!ok) {
            std::cout << "Load json error: " << errs << std::endl;
            return false;
        }
        return true;
    }

    static bool Save(const std::string &filename, Json::Value &root)
    {
        std::ofstream os(filename);
        if (!os.is_open()) {
            std::cout << "open " << filename << " failed." << std::endl;
            return false;
        }
        Json::StreamWriterBuilder wbuilder;
        std::unique_ptr<Json::StreamWriter> jsonWriter(wbuilder.newStreamWriter());
        jsonWriter->write(root, &os);
        os.close();
        return true;
    }
};