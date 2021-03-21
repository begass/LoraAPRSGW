/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ConfigParam.cpp
 * Author: paulalexander66
 * 
 * Created on 05. Mai 2017, 12:16
 */

#include "ConfigParam.h"

ConfigParam::ConfigParam(){
    m_sParam= "";
    m_sValue="";
}

ConfigParam::ConfigParam(string Param, string Value) {
    m_sParam= Param;
    m_sValue=Value;
   
}

ConfigParam::~ConfigParam() {
}

string ConfigParam::get_Param(void){
    return m_sParam;
}

string ConfigParam::get_Value(void){
    return m_sValue;
}

void ConfigParam::copyConfigParam(ConfigParam object){
    m_sParam=object.get_Param();
    m_sValue=object.get_Value();
}
