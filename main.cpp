#include "mbed.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#define UINT14_MAX        16383
 // FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0
#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0
#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1
#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1
 // FXOS8700CQ internal register addresses
#define FXOS8700Q_STATUS 0x00
#define FXOS8700Q_OUT_X_MSB 0x01
#define FXOS8700Q_OUT_Y_MSB 0x03
#define FXOS8700Q_OUT_Z_MSB 0x05
#define FXOS8700Q_M_OUT_X_MSB 0x33
#define FXOS8700Q_M_OUT_Y_MSB 0x35
#define FXOS8700Q_M_OUT_Z_MSB 0x37
#define FXOS8700Q_WHOAMI 0x0D
#define FXOS8700Q_XYZ_DATA_CFG 0x0E
#define FXOS8700Q_CTRL_REG1 0x2A
#define FXOS8700Q_M_CTRL_REG1 0x5B
#define FXOS8700Q_M_CTRL_REG2 0x5C
#define FXOS8700Q_WHOAMI_VAL 0xC7

I2C i2c( PTD9,PTD8);
Serial pc(USBTX, USBRX);
int m_addr = FXOS8700CQ_SLAVE_ADDR1;
DigitalOut led1(LED1);

InterruptIn sw2(SW2);
 
EventQueue queue1(32 * EVENTS_EVENT_SIZE);
//EventQueue queue2(32 * EVENTS_EVENT_SIZE);
Thread thread1;
//Thread thread2;

void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);
void FXOS8700CQ_writeRegs(uint8_t * data, int len);
 /*
void led1_thread()  {
    for(int i=0;i<10;i++){
    led1 = !led1;
    wait(0.5);
    led1 = !led1;
    wait(0.5);
    }    
} */
 
void acc_call()  {
    pc.baud(115200);
 
    uint8_t who_am_i, data[2], res[6];
    int16_t acc16;
    float t[3];
 
   // Enable the FXOS8700Q

    FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
    data[1] |= 0x01;
    data[0] = FXOS8700Q_CTRL_REG1;
    FXOS8700CQ_writeRegs(data, 2);

    // Get the slave address
    FXOS8700CQ_readRegs(FXOS8700Q_WHOAMI, &who_am_i, 1);
 
    //uLCD.printf("Here is %x\r\n", who_am_i);
    //while (true) {
    
    for(int i = 0;i<100;i++){
        
        FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);
 
        acc16 = (res[0] << 6) | (res[1] >> 2);
        if (acc16 > UINT14_MAX/2)
            acc16 -= UINT14_MAX;
        t[0] = ((float)acc16) / 4096.0f;
 
        acc16 = (res[2] << 6) | (res[3] >> 2);
        if (acc16 > UINT14_MAX/2)
            acc16 -= UINT14_MAX;
        t[1] = ((float)acc16) / 4096.0f;

        acc16 = (res[4] << 6) | (res[5] >> 2);
        if (acc16 > UINT14_MAX/2)
            acc16 -= UINT14_MAX;
        t[2] = ((float)acc16) / 4096.0f;
 
        pc.printf("%1.4f   %1.4f   %1.4f   \r\n",\
             t[0],t[1],t[2]
        );
        
        if(i%10==0)
        led1 = !led1;
        wait(0.1);
    }
}

void acc_thread(){
    
    queue1.call(acc_call);
    //led1 = !led1;
    //wait(1.0);
    //}
}

int main() {
    thread1.start(callback(&queue1, &EventQueue::dispatch_forever));
    //thread2.start(callback(&queue2, &EventQueue::dispatch_forever));
 
 
    //sw2.rise(queue1.event(led1_thread));
 
    sw2.rise(acc_thread);
}


void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {
    char t = addr;
    i2c.write(m_addr, &t, 1, true);
    i2c.read(m_addr, (char *)data, len);
}

void FXOS8700CQ_writeRegs(uint8_t * data, int len) {
    i2c.write(m_addr, (char *)data, len);
}