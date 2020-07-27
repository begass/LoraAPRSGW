/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ParamList.h
 * Author: paulalexander66
 *
 * Created on 05. Mai 2017, 14:26
 */

#ifndef PARAMLIST_H
#define PARAMLIST_H

#include "ConfigParam.h"
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>

using namespace std;

class ParamList {
public:
    ParamList();
    ParamList(const ParamList& orig);
    virtual ~ParamList();
    int openConfig(string Path);
    int size();
    void listAllParam();
    string getValue(string Param);
    ConfigParam getConfigAt(int Number);
    
private:
    string m_sPath;
    vector<ConfigParam> m_ParamList;
};

#endif /* PARAMLIST_H */

