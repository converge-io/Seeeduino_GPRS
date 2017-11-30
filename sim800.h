/*
 * sim800.h
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

#ifndef __SIM800_H__
#define __SIM800_H__

#include <mbed.h>
#include "SEGGER_RTT.h"


#define TRUE                    1
#define FALSE                   0

#define SIM800_TX_PIN           p11
#define SIM800_RX_PIN           p12
#define SIM800_RESET_PIN        p24

#define UART_DEBUG

#ifdef UART_DEBUG
#define ERROR(x)            SEGGER_RTT_printf(0,x);SEGGER_RTT_printf(0,"\n");
#define DEBUG(x)            SEGGER_RTT_printf(0,x);SEGGER_RTT_printf(0,"\n");
#else
#define ERROR(x)
#define DEBUG(x)
#endif

#define DEFAULT_TIMEOUT     5

/** SIM800 class.
 *  Used for SIM800 communication. attention that SIM800 module communicate with MCU in serial protocol
 */

class SIM800
{

public:
    /** Create SIM800 Instance
     *  @param tx   uart transmit pin to communicate with SIM800
     *  @param rx   uart receive pin to communicate with SIM800
     *  @param baudRate baud rate of uart communication
     */
    SIM800(int baudRate):serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN),resetPin(SIM800_RESET_PIN){
        serialSIM800.baud(baudRate);
        resetPin.input();
        resetPin.mode(PullNone);
    };

    /** Power on SIM800
     */
    void preInit(void);

    /** Check if SIM800 readable
     */
    int checkReadable(void);

    /** read from SIM800 module and save to buffer array
     *  @param  buffer  buffer array to save what read from SIM800 module
     *  @param  count   the maximal bytes number read from SIM800 module
     *  @param  timeOut time to wait for reading from SIM800 module
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int readBuffer(char* buffer,int count, unsigned int timeOut = DEFAULT_TIMEOUT);


    /** clean Buffer
     *  @param buffer   buffer to clean
     *  @param count    number of bytes to clean
     */
    void cleanBuffer(char* buffer, int count);

    /** send AT command to SIM800 module
     *  @param cmd  command array which will be send to GPRS module
     */
    void sendCmd(const char* cmd);

    /**send "AT" to SIM800 module
     */
    int sendATTest(void);

    /**send '0x1A' to SIM800 Module
     */
    void sendEndMark(void);

    /** check SIM800 module response before time out
     *  @param  *resp   correct response which SIM800 module will return
     *  @param  *timeout    waiting seconds till timeout
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int waitForResp(const char* resp, unsigned timeout);

    /** send AT command to GPRS module and wait for correct response
     *  @param  *cmd    AT command which will be send to GPRS module
     *  @param  *resp   correct response which GPRS module will return
     *  @param  *timeout    waiting seconds till timeout
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int sendCmdAndWaitForResp(const char* cmd, const char *resp, unsigned timeout);

    Serial serialSIM800;
    DigitalInOut resetPin;

private:
  Timer timeoutTimer;
};

#endif
