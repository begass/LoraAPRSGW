/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ConfigParam.h
 * Author: paulalexander66
 *
 * Created on 05. Mai 2017, 12:16
 */

#ifndef CONFIGPARAM_H
#define CONFIGPARAM_H

//#include <cstdlib>
#include <string>

using namespace std;

class ConfigParam {
public:
    ConfigParam();
    ConfigParam(string Param, string Value);
    virtual ~ConfigParam();
    string get_Param(void);
    string get_Value(void);
    void copyConfigParam(ConfigParam object);
private:
    string m_sParam;
    string m_sValue;
};

#endif /* CONFIGPARAM_H */

