#include <msp430.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "SPI.h"
#include "LORA.h"
#include "FSM.h"
#include "UART.h"
#include "help.h"
#include "LED.h"
#include "gps.h"

void main(void){
    configureGPS();
}
