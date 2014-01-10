/*
* gprs.cpp 
* A library for SeeedStudio seeeduino GPRS shield 
*  
* Copyright (c) 2013 seeed technology inc. 
* Author      	: 	lawliet.zou(lawliet.zou@gmail.com)
* Create Time	: 	Dec 23, 2013 
* Change Log 	: 	
*
* The MIT License (MIT)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include <stdio.h>
#include "gprs.h"

int GPRS::init(void)
{	
	for(int i = 0; i < 2; i++){
		sendCmd("AT\r\n");
		delay(100);
	}
	sendCmd("AT+CFUN=1\r\n"); 
    if(0 != checkSIMStatus()) {
        ERROR("ERROR:checkSIMStatus");
        return -1;
    }
    return 0;
}

int GPRS::checkSIMStatus(void)
{
    char gprsBuffer[32];
    int count = 0;
    cleanBuffer(gprsBuffer,32);
    while(count < 3) {
        sendCmd("AT+CPIN?\r\n");
        readBuffer(gprsBuffer,32,DEFAULT_TIMEOUT);
        if((NULL != strstr(gprsBuffer,"+CPIN: READY"))) {
            break;
        }
        count++;
        delay(1000);
    }
    if(count == 3) {
        return -1;
    }
    return 0;
}

int GPRS::networkCheck(void)
{
	delay(1000);
    if(0 != sendCmdAndWaitForResp("AT+CGREG?\r\n","+CGREG: 0,1",DEFAULT_TIMEOUT*3)) { 
        ERROR("ERROR:CGREG");
        return -1;
    }
    delay(1000);
    if(0 != sendCmdAndWaitForResp("AT+CGATT?\r\n","+CGATT: 1",DEFAULT_TIMEOUT)) {
        ERROR("ERROR:CGATT");
        return -1;
    }
    return 0;
}

int GPRS::sendSMS(char *number, char *data)
{
    char cmd[32];
    if(0 != sendCmdAndWaitForResp("AT+CMGF=1\r\n", "OK", DEFAULT_TIMEOUT)) { // Set message mode to ASCII
        ERROR("ERROR:CMGF");
        return -1;
    }
    delay(500);
    snprintf(cmd, sizeof(cmd),"AT+CMGS=\"%s\"\r\n", number);
    if(0 != sendCmdAndWaitForResp(cmd,">",DEFAULT_TIMEOUT)) {
        ERROR("ERROR:CMGS");
        return -1;
    }
    delay(1000);
    serialSIM800.write(data);
    delay(500);
	sendEndMark();
	return 0;
}

int GPRS::readSMS(int messageIndex, char *message,int length)
{
	int i = 0;
    char gprsBuffer[100];
	char cmd[16];
    char *p,*s;
	
	sendCmdAndWaitForResp("AT+CMGF=1\r\n","OK",DEFAULT_TIMEOUT);
	delay(1000);
	sprintf(cmd,"AT+CMGR=%d\r\n",messageIndex);
	serialSIM800.write(cmd);
    cleanBuffer(gprsBuffer,100);
    readBuffer(gprsBuffer,100,DEFAULT_TIMEOUT);

    if(NULL != ( s = strstr(gprsBuffer,"+CMGR"))){
        if(NULL != ( s = strstr(gprsBuffer,"+32"))){
            p = s + 6;
            while((*p != '$')&&(i < length-1)) {
                message[i++] = *(p++);
            }
            message[i] = '\0';
        }
    }
    return 0;
}

int GPRS::deleteSMS(int index)
{
    char cmd[16];
    snprintf(cmd,sizeof(cmd),"AT+CMGD=%d\r\n",index);
    sendCmd(cmd);
    return 0;
}

int GPRS::callUp(char *number)
{
	char cmd[24];
    if(0 != sendCmdAndWaitForResp("AT+COLP=1\r\n","OK",5)) {
        ERROR("COLP");
        return -1;
    }
    delay(1000);
	sprintf(cmd,"\r\nATD%s;\r\n", number);
	serialSIM800.write(cmd);
    return 0;
}

int GPRS::answer(void)
{
    serialSIM800.write("ATA\r\n");
    return 0;
}

int GPRS::connectTCP(const char *ip, int port)
{
    char cipstart[50];
    sprintf(cipstart, "AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r\n", ip, port);
    if(0 != sendCmdAndWaitForResp(cipstart, "CONNECT OK", 10)) {// connect tcp
        ERROR("ERROR:CIPSTART");
        return -1;
    }

    return 0;
}
int GPRS::sendTCPData(char *data)
{
    char cmd[32];
    int len = strlen(data);
    snprintf(cmd,sizeof(cmd),"AT+CIPSEND=%d\r\n",len);
    if(0 != sendCmdAndWaitForResp(cmd,">",2*DEFAULT_TIMEOUT)) {
        ERROR("ERROR:CIPSEND");
        return -1;
    }
    if(0 != sendCmdAndWaitForResp(data,"SEND OK",2*DEFAULT_TIMEOUT)) {
        ERROR("ERROR:SendTCPData");
        return -1;
    }
    return 0;
}

int GPRS::closeTCP(void)
{
    sendCmd("AT+CIPCLOSE\r\n");
    return 0;
}

int GPRS::shutTCP(void)
{
    sendCmd("AT+CIPSHUT\r\n");
    return 0;
}



