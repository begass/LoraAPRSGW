/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ParamList.cpp
 * Author: paulalexander66
 * 
 * Created on 05. Mai 2017, 14:26
 */

#include "ParamList.h"

ParamList::ParamList() {
    m_sPath="";
}

ParamList::ParamList(const ParamList& orig) {
}

ParamList::~ParamList() {
}

 int ParamList::openConfig(string Path){
    m_sPath=Path; 
    ifstream conf_file(m_sPath.c_str());
    if(!conf_file){
        //printf("File could not be opened..\n");
        conf_file.close() ;
        m_sPath="";
        return 1;
    }
    //printf( "Open File \n");
    
    
    string line;
    while (getline(conf_file, line)) {
        if(!line.find("#")==0 ){
            int iFound =line.find(":");
            if(iFound>0){
                //printf("%s",line.c_str());
                //printf("\":\" found at %d",iFound);
                //printf("\n");
                while(line.find(" ")==0){
                    line=line.substr(1);
                }
                    
                string sParam = line.substr(0,iFound);
                string sValue=line.substr(iFound+1,line.length());
                //printf("Parameter %s \t Value %s \n", sParam.c_str(),sValue.c_str());
                ConfigParam *s;
                s=new ConfigParam(sParam,sValue);
                m_ParamList.push_back(*s);
                
                
            }
        }
    }
    conf_file.close() ; 
    
     return 0;
 }
 
 int ParamList::size(){
     return m_ParamList.size();
 }
 
 void ParamList::listAllParam(){
     for (u_int i = 0; i<m_ParamList.size();i++){
        ConfigParam x;
        x.copyConfigParam(m_ParamList.at(i));
        printf("Param is %s : ",x.get_Param().c_str());
        printf("Value is %s \n",x.get_Value().c_str());       
    }
 }

ConfigParam ParamList::getConfigAt(int Number){
     return m_ParamList.at(Number);          
}

 string ParamList::getValue(string Param){
     string sReturn="";
     for (u_int i = 0; i<m_ParamList.size();i++){
        ConfigParam x;
        x.copyConfigParam(m_ParamList.at(i));
        if(x.get_Param().compare(Param)==0){
            //printf("Param is %s : ",x.get_Param().c_str());
            //printf("Value is %s \n",x.get_Value().c_str());
            sReturn=x.get_Value();
        }
    }
     return sReturn;
 }
