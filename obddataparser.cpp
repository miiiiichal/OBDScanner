#include "obddataparser.h"


ObdDataParser::ObdDataParser()
{

}

std::vector<QString> ObdDataParser::decodeDTC(const std::vector<QString> &hex_vals)
{
/*
43 12 29 03 80 00 00

12 29 03 80=>
P1229
P0	380
*/
    std::vector<QString> dtc_codes;
    QString dtc_code;
    if(hex_vals.size()>0 && !hex_vals[0].compare("43", Qt::CaseInsensitive)){
        QString tmp_code;
        for(int it=1;it<hex_vals.size();++it){
            if(it%2!=0){
                tmp_code.append(hex_vals[it]);
                char x=tmp_code.toStdString()[0];
                auto itMap = this->dtcPrefix.find(x);
                dtc_code.append((*itMap).second);
                dtc_code.append(tmp_code[1]);
            }
            else{
                dtc_code.append(hex_vals[it]);
                if(dtc_code.compare("P0000", Qt::CaseInsensitive)!=0)
                    dtc_codes.push_back(dtc_code);
                tmp_code.clear();
                dtc_code.clear();
            }
        }
    }
    return dtc_codes;
}

std::vector<QString> ObdDataParser::prepareResponseToDecode(const QString &response_str)
{
//std::string str("41  0C  81 33 00 ");
//010C\rSEARCHING...\r...41 0C 00 00 \r\r>

// !!!! 0101>\r 41 01 82 00 00 00\r\r> !!!! not working
std::vector<QString> result;
result.reserve(8);
std::string str(response_str.toStdString());

std::replace(str.begin(),str.end(),'>',' ');
size_t pos = str.find("\r\r",0);
if(pos!=std::string::npos)
    str.resize(pos);
if(str[0]=='A' && str[1]=='T'){
    result.push_back(response_str);
}

std::replace(str.begin(),str.end(),'\r',' ');
if(str.find("NO DATA",0)!=std::string::npos){
    //std::cout<<"NODATA";
    result.push_back(QString("NO DATA"));
    return result;
}

pos=str.find("SEARCHING",0);
if(pos!=std::string::npos){
    //usunac searching
   str=str.substr(pos+9,str.length()-pos);
   pos=str.find_last_of(".",0);
   if(pos!=std::string::npos){
        str=str.substr(pos);
   }
 std::replace(str.begin(),str.end(),'.',' ');
}

int counter=0;
size_t start=0;
    while(str.length()>=2){
        //std::cout<<str[0]<<std::endl;
        if(str[0]!=' '){
            std::string hex_str=str.substr(0,2);
            result.push_back(QString(hex_str.c_str()));
            //std::cout<<"hexstr:"<<hex_str<<std::endl;
            int i_hex = std::stoi(hex_str,nullptr,16);
          //  std::cout<<"hexval:"<<i_hex<<std::endl;
        }
        start=str.find(" ",0);
        if(start==std::string::npos)
            str="";
        else
            str=str.substr(start+1);
        //std::cout<<"new str: "<<str<<std::endl;
        ++counter;
        if(counter>100)
        break;
    }
return result;
}

int ObdDataParser::decodeKmHSpeed(const std::vector<QString> &hex_vals){
    //010D -> A [0-255]
    int speed =0;
    if(hex_vals.size()>0){
        speed = std::stoi(hex_vals[0].toStdString(),nullptr,16);
    }
return speed;
}
int ObdDataParser::decodeEngineRPM(const std::vector<QString> &hex_vals){
    //010C -> (256A+B)/4
    int rpm =0;
    if(hex_vals.size()>=2){
        int A = std::stoi(hex_vals[0].toStdString(),nullptr,16);
        int B = std::stoi(hex_vals[1].toStdString(),nullptr,16);
        rpm = (256*A +B) / 4;
    }
   // 0B FC => [11 252] = (256*11 +252)/4 = 767
   // 0C E8 => [12 232] = 826;
    return rpm;
}
int ObdDataParser::decodeCoolantTemp(const std::vector<QString> &hex_vals){
    //0105 -> result-40
    int temp=-99;
    if(hex_vals.size()>0){
        temp = std::stoi(hex_vals[0].toStdString(),nullptr,16);
        if(temp>=0 && temp<=255)
            temp=temp-40;
    }
return temp;
}

int ObdDataParser::decodeIntakeAirTemp(const std::vector<QString> &hex_vals){
    //010f -> result-40
    int temp=-99;
    if(hex_vals.size()>0){
        temp = std::stoi(hex_vals[0].toStdString(),nullptr,16);
        if(temp>=0 && temp<=255)
            temp=temp-40;
    }
return temp;
}

int ObdDataParser::decodeVehicleIdNumber(const std::vector<QString> &hex_vals){
    return 0;
}

std::pair<int,bool> ObdDataParser::decodeNumberOfDtc(const std::vector<QString> &hex_vals)
{
    //82 00 00 00
    int dtcNumber=0;
    bool milOn= false;
    int number= std::stoi(hex_vals[0].toStdString(),nullptr,16);
    if(number-128 < 0) {
        dtcNumber=number;
    }
    else{
        dtcNumber=number-128;
        milOn=true;
    }
    return std::make_pair(dtcNumber,milOn);

}




// 012F = Fuel Tank Level Input	0-100 % ->	100/255 * A

// 015E - Engine fuel rate {256A+B}\{20}

//0152 - fuel type
/*
    Description
0	Not available
1	Gasoline
2	Methanol
3	Ethanol
4	Diesel
5	LPG
6	CNG
7	Propane
8	Electric
9	Bifuel running Gasoline
10	Bifuel running Methanol
11	Bifuel running Ethanol
12	Bifuel running LPG
13	Bifuel running CNG
14	Bifuel running Propane
15	Bifuel running Electricity
16	Bifuel running electric and combustion engine
17	Hybrid gasoline
18	Hybrid Ethanol
19	Hybrid Diesel
20	Hybrid Electric
21	Hybrid running electric and combustion engine
22	Hybrid Regenerative
23	Bifuel running diesel
*/


// 011C - OBD standard conforms to:
/*
1	OBD-II as defined by the CARB
2	OBD as defined by the EPA
3	OBD and OBD-II
4	OBD-I
5	Not OBD compliant
6	EOBD (Europe)
7	EOBD and OBD-II
8	EOBD and OBD
9	EOBD, OBD and OBD II
10	JOBD (Japan)
11	JOBD and OBD II
12	JOBD and EOBD
13	JOBD, EOBD, and OBD II
14	Reserved
15	Reserved
16	Reserved
17	Engine Manufacturer Diagnostics (EMD)
18	Engine Manufacturer Diagnostics Enhanced (EMD+)
19	Heavy Duty On-Board Diagnostics (Child/Partial) (HD OBD-C)
20	Heavy Duty On-Board Diagnostics (HD OBD)
21	World Wide Harmonized OBD (WWH OBD)
22	Reserved
23	Heavy Duty Euro OBD Stage I without NOx control (HD EOBD-I)
24	Heavy Duty Euro OBD Stage I with NOx control (HD EOBD-I N)
25	Heavy Duty Euro OBD Stage II without NOx control (HD EOBD-II)
26	Heavy Duty Euro OBD Stage II with NOx control (HD EOBD-II N)
27	Reserved
28	Brazil OBD Phase 1 (OBDBr-1)
29	Brazil OBD Phase 2 (OBDBr-2)
30	Korean OBD (KOBD)
31	India OBD I (IOBD I)
32	India OBD II (IOBD II)
33	Heavy Duty Euro OBD Stage VI (HD EOBD-IV)
34-250	Reserved
251-255	Not available for assignment (SAE J1939 special meaning)



01 21 - >Distance traveled with malfunction indicator lamp (MIL) on ->256A+B

*/
