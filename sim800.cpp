/*
 * sim800.cpp
 * A library for SeeedStudio seeeduino GPRS shield
 *
 * Copyright (c) 2013 seeed technology inc.
 * Author        :   lawliet zou
 * Create Time   :   Dec 2013
 * Change Log    :
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

#include "sim800.h"

void SIM800::preInit(void)
{
    //If we can't talk to the module, reset it
    if(sendATTest() != 0)
    {
        resetPin.output();
        resetPin = 0;
        wait(2);  //Pull the reset low for 2 seconds
        resetPin.input();
        wait(3); //Release and let it pull itself high again.
    }
    while(sendATTest() != 0);
    SEGGER_RTT_printf(0,"Pre-Init O.K!\n");
}

int SIM800::checkReadable(void)
{
    return serialSIM800.readable();
}

int SIM800::readBuffer(char *buffer,int count, unsigned int timeOut)
{
    int i = 0;
    unsigned long timerStart,timerEnd;
    timeoutTimer.reset();
    timeoutTimer.start();
    timerStart = timeoutTimer.read_ms();

    while(1) {
        SEGGER_RTT_printf(0,"1\n");
        while (serialSIM800.readable()) {
            char c = serialSIM800.getc();
            if (c == '\r' || c == '\n') c = '$';
            buffer[i++] = c;
            if(i > count-1)break;
        }
        if(i > count-1)break;
        timerEnd = timeoutTimer.read_ms();
        if(timerEnd - timerStart > 1000 * timeOut) {
            break;
        }
    }
    SEGGER_RTT_printf(0,"2\n");
    SEGGER_RTT_printf(0,"3\n");
    //wait(0.5);

    while(serialSIM800.readable()) {   // display the other thing..
      SEGGER_RTT_printf(0,"4\n");
      serialSIM800.getc();
    }
    SEGGER_RTT_printf(0,"5\n");
    return 0;
}

void SIM800::cleanBuffer(char *buffer, int count)
{
    for(int i=0; i < count; i++) {
        buffer[i] = '\0';
    }
}

void SIM800::sendCmd(const char* cmd)
{
    serialSIM800.printf(cmd);
}

int SIM800::sendATTest(void)
{
    int ret = sendCmdAndWaitForResp("AT\r\n","OK",DEFAULT_TIMEOUT);
    return ret;
}

int SIM800::waitForResp(const char *resp, unsigned int timeout)
{
    int len = strlen(resp);
    int sum=0;
    unsigned long timerStart,timerEnd;
    timeoutTimer.reset();
    timeoutTimer.start();
    timerStart = timeoutTimer.read_ms();
    while(1) {
        if(serialSIM800.readable()) {
            char c = serialSIM800.getc();
            sum = (c==resp[sum]) ? sum+1 : 0;
            if(sum == len)break;
        }
        timerEnd = timeoutTimer.read_ms();
        if(timerEnd - timerStart > 1000 * timeout) {
            return -1;
        }
    }

    while(serialSIM800.readable()) {
        serialSIM800.getc();
    }

    return 0;
}

void SIM800::sendEndMark(void)
{
    serialSIM800.putc((char)26);
}


int SIM800::sendCmdAndWaitForResp(const char* cmd, const char *resp, unsigned timeout)
{
    sendCmd(cmd);
    return waitForResp(resp,timeout);
}
