#include <streambuf>
#include <sstream>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <pthread.h>
#include "fft/fft.h"
#include <math.h>
#include <webservice.h>
#include "base64B.h"
#include <string>
#include <cstring>
#include <stdio.h>
#define FFTPoint 128
using namespace std;
using namespace rapidjson;
int id;
int income_id;
uint8_t sBuf[1024];
float analogData[FFTPoint];
int analog_index=0;
string path,path1;
int mode;
bool fftOn=false;

 int get_id()
 {
     id=id+1;
     if (id>5000) id=0;
     return id;    
 }
void fft_convert()
 {
     int i;
    complex *pSignal = new complex[FFTPoint];
      for(i=0; i<FFTPoint; i++)
    {
        pSignal[i] = analogData[i];
    }
     CFFT::Forward(pSignal,FFTPoint);
      for(i=0; i<FFTPoint; i++)
    {
        pSignal[i] = sqrt(pSignal[i].re()*pSignal[i].re()+pSignal[i].im()*pSignal[i].im()); 
        analogData[i]= pSignal[i].re();    
    } 
    delete [] pSignal;
 }

void registerApp(string appName,int port)
{
   string path="000/000/000";
    Document message;
    message.SetObject();
    Document::AllocatorType& allocator = message.GetAllocator();   
     message.AddMember("method","register.app",allocator);
      message.AddMember("mode",1,allocator);
     Value vstring(kStringType);
      vstring.SetString(path.c_str(),path.size(), allocator); 
      message.AddMember("path",vstring,allocator); 
     Value params(kObjectType);
      vstring.SetString(appName.c_str(),appName.size(), allocator); 
     params.AddMember("name",vstring,allocator); 
      params.AddMember("port",port,allocator); 
     message.AddMember("params",params,allocator); 
     message.AddMember("id",get_id(),allocator);
     StringBuffer buffer;
    buffer.Clear();
    Writer<StringBuffer> writer(buffer);
    message.Accept(writer);
    string msg_payload=buffer.GetString();
  //  cout<<"msg payload: "<<msg_payload<<"\n"<<endl;
    sendMessage(msg_payload); 
}
void send_response() 
{ int i,k;
Document message;
    message.SetObject();   
    Document::AllocatorType& allocator = message.GetAllocator();  
     Value vstring(kStringType);
     vstring.SetString(path.c_str(),path.size(), allocator); 
    message.AddMember("path",vstring,allocator);
    Value result(kObjectType);	 
    result.AddMember("status", "readFFT",allocator);
    // Value data(kArrayType);
    int index=0;
     for (i=0;i<FFTPoint;i++)
     { 
        uint8_t temp[4];
        memcpy(temp,&analogData[i],4);
        for (k=0;k<4;k++)
             sBuf[index++]=temp[k];

     };
     string data64=base64_encodeB(sBuf,index); 
     Value data(kStringType);
     data.SetString(data64.c_str(),allocator);
    result.AddMember("value",data,allocator);             
    message.AddMember("result",result,allocator);
    message.AddMember("id",income_id,allocator);
     StringBuffer buffer;
    buffer.Clear();
    Writer<StringBuffer> writer(buffer);
    message.Accept(writer);
    string msg_payload=buffer.GetString();
 // cout<<"msg payload: "<<msg_payload<<"\n"<<endl;
    sendMessage(msg_payload); 
}
void registerRPC()
{  path="000/000/000";
   path1="000/002/000";
    Document message;
    message.SetObject();
    Document::AllocatorType& allocator = message.GetAllocator();   
     message.AddMember("method","register.rpc",allocator);
      message.AddMember("mode",1,allocator);
     Value vstring(kStringType);
      vstring.SetString(path.c_str(),path.size(), allocator); 
      message.AddMember("path",vstring,allocator); 
     Value params(kObjectType);
     params.AddMember("name","readFFT",allocator); 
      vstring.SetString(path1.c_str(),path1.size(), allocator);
       params.AddMember("path",vstring,allocator);  
     message.AddMember("params",params,allocator); 
     message.AddMember("id",get_id(),allocator);
     StringBuffer buffer;
    buffer.Clear();
    Writer<StringBuffer> writer(buffer);
    message.Accept(writer);
    string msg_payload=buffer.GetString();
   // cout<<"msg payload: "<<msg_payload<<"\n"<<endl;
    sendMessage(msg_payload); 
}
void startADC(int pin)
{  path="000/000/001";
    Document message;
    message.SetObject();
    Document::AllocatorType& allocator = message.GetAllocator();   
     message.AddMember("method","analog.start",allocator); 
      message.AddMember("mode",2,allocator);
      Value vstring(kStringType);
      vstring.SetString(path.c_str(),path.size(), allocator); 
      message.AddMember("path",vstring,allocator); 
     Value params(kObjectType);
     int id=get_id();
     Value data(kArrayType);
      data.PushBack(1, allocator);
     params.AddMember("value",data,allocator); 
     message.AddMember("params",params,allocator); 
     message.AddMember("id",id,allocator);
     StringBuffer buffer;
    buffer.Clear();
    Writer<StringBuffer> writer(buffer);
    message.Accept(writer);
    string msg_payload=buffer.GetString();
 //  cout<<"msg payload: "<<msg_payload<<"\n"<<endl;
    sendMessage(msg_payload); 
}
void webAPI(string request,string *response)
{
    Document req;
    req.Parse(request.c_str());
    cout<<"jsonstring method:"<<req["method"].GetString()<<endl;
    *response="{\"result\":\"OK\"}";
}
void webOnMessage(string request,string *response)
{
 //TODO
}
void baseOnMessage(string request)
{
     int i,p;
    string methodName;
    connected=true;
    Document req;
    Value result(kObjectType);
    req.Parse(request.c_str());

   if (req.HasMember("mode"))
   {
       mode=req["mode"].GetInt();
   }
   if( req.HasMember("path"))
    {
        path=req["path"].GetString();
    }
    if (req.HasMember("method"))
    {
        methodName=req["method"].GetString();
         if (methodName.compare("welcome")==0) 
         {
             registerApp("vibration",9102);
             registerRPC();
             startADC(1);
         }else
          if (methodName.compare("readFFT")==0) 
         {
             
            income_id=req["id"].GetInt();
            cout<<" readFFT income id : "<<income_id<<endl;
            if (mode==2)
                { fftOn=true;}
            else
              fftOn=false;
          }
    }
     if (req.HasMember("result"))
     {
        result =  req["result"];
        if (result.HasMember("status")) 
        {
            string status=result["status"].GetString();
            if (status.compare("analog.start")==0)
            { 
               
                if (result.HasMember("value"))
                {
                    // const rapidjson::Value& data =result["value"];
                /* p=0;
                  /   for (rapidjson::Value::ConstValueIterator itr = data.Begin(); itr != data.End(); ++itr) 
                        {  
                            const rapidjson::Value& d = *itr;
                            if (d.IsInt())
                            {
                                sBuf[p++]=d.GetInt();
                              //  cout<<"d="<<d.GetInt()<<endl;
                            }       
                        }*/
                     string data64=result["value"].GetString();
                    // cout<<"raw length: "<<data64.length()<<endl;
                     string rawdata=base64_decodeB(data64);
                      //  int length=p
                    //  cout<<"length: "<<rawdata.length()<<endl;
                      int length=rawdata.length();
                      memcpy(sBuf,rawdata.c_str(),length);
                        p=0;
                       for (i=0;i<(length/4);i++) 
                       {
                          memcpy(&analogData[analog_index++],&sBuf[p],4);
                          p=p+4;
                       } 
                       if (analog_index==FFTPoint) 
                       {
                           analog_index=0;
                           fft_convert();
                           if (fftOn)
                           send_response();
                       } 
                }
               
            }
        }
     }
}
 
int main()
{
   cout<<"BaseService  App (vibration)"<<endl;
    init(9102);
}