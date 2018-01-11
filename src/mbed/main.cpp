#include "mbed.h"
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <math.h> 
#define REAL double
 
DigitalOut led1(LED1);
DigitalOut led2(LED2);
 
PwmOut servo(p21);  //servo 0.0005 to 0.002
PwmOut servo2(p22); //servo 0.0005 to 0.002
PwmOut servo3(p23); //servo 0.0005 to 0.002
PwmOut servo4(p24); //servo 0.0005 to 0.002
AnalogIn voltage(p20);
Serial pc(USBTX, USBRX); // tx, rx
Serial device(p9, p10);  // tx, rx

float lastVoltage=0,compareV=0,offsetTest,currentVoltage,lastVoltage2=0,currentVoltage2,offset=0.0005,offset2=0.0005,offsetOld=0.0,offset2Old=0.0,val1,val2;
int lastCommand=1,currentCommand=1;
bool sysOff=false;

void init_servo(){ // initializing the servo
    pc.printf("System initializing\r\n");
    servo.period(0.020);          // servo requires a 20ms period
    servo2.period(0.020);          // servo requires a 20ms period
    servo.pulsewidth(0.0005);
    servo2.pulsewidth(0.001);
    wait(1);
    
    for(float offsetTest=0.0; offsetTest<0.0015; offsetTest+=0.0001) {
            servo.pulsewidth(0.0005 + offsetTest); // servo position determined by a pulsewidth between 1-2ms
            wait(0.6);
            currentVoltage=(voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read())/10;
            if (compareV<currentVoltage) {compareV=currentVoltage;offset=offsetTest;}  // use the largest voltage position
            pc.printf("offsetTest: %f    offset: %f\r\n",offsetTest,offset);

    }
        
    compareV=0.0; 
    servo.pulsewidth(0.0005 + offset);
    //servo2.pulsewidth(0.0005);   
    for(float offsetTest=0.0; offsetTest<0.0015; offsetTest+=0.0001) {
            servo2.pulsewidth(0.0005 + offsetTest); // servo position determined by a pulsewidth between 1-2ms
            wait(0.6);
            currentVoltage=(voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read())/10;
            if (compareV<currentVoltage) {compareV=currentVoltage;offset2=offsetTest;} // use the largest voltage position
            pc.printf("offsetTest: %f    offset2: %f\r\n",offsetTest,offset2);

    }
    pc.printf("Initialization complete\r\n");
}

void callback() {  // receive from Beaglebone
    // read from the serial to clear the RX interrupt
    currentCommand=((int)(device.getc()- '0'));
    //pc.printf("currentCommand: %d\r\n", currentCommand);
    if (currentCommand!=lastCommand) {if (currentCommand==1) {sysOff=false;pc.printf("Releasing Panels!\r\n");init_servo();} else {sysOff=true;offset=0;offset2=0;pc.printf("Retracting Panels!\r\n");}}
    if (currentCommand==0) {offset=0;offset2=0;servo.pulsewidth(0.0005 + offset);servo2.pulsewidth(0.0005 + offset2);servo3.pulsewidth(0.0005 + offset);servo4.pulsewidth(0.0005 + offset2);}
    //pc.printf("%c", device.getc());
    lastCommand=currentCommand;
    led2 = !led2;  // toggle led
}
 

inline static REAL sqr(REAL x) {
    return x*x;
}
    
int linreg(int n, const REAL x[], const REAL y[], REAL* m, REAL* b, REAL* r)
    {
        REAL   sumx = 0.0;                        /* sum of x                      */
        REAL   sumx2 = 0.0;                       /* sum of x**2                   */
        REAL   sumxy = 0.0;                       /* sum of x * y                  */
        REAL   sumy = 0.0;                        /* sum of y                      */
        REAL   sumy2 = 0.0;                       /* sum of y**2                   */

       for (int i=0;i<n;i++)   // executing math calculation
          { 
          sumx  += x[i];       
          sumx2 += sqr(x[i]);  
          sumxy += x[i] * y[i];
          sumy  += y[i];      
          sumy2 += sqr(y[i]); 
          } 

       REAL denom = (n * sumx2 - sqr(sumx));
       if (denom == 0) { // rule out invalid data
           *m = 0;
           *b = 0;
           if (r) *r = 0;
           return 1;
       }

       *m = (n * sumxy  -  sumx * sumy) / denom;   // m is the slope
       *b = (sumy * sumx2  -  sumx * sumxy) / denom;
       if (r!=NULL) {                 // calculating r
          *r = (sumxy - sumx * sumy / n) /        
                sqrt((sumx2 - sqr(sumx)/n) *
                (sumy2 - sqr(sumy)/n));
       }

       return 0; 
}










float search(bool direction,float val) // search for horizontal direction
    {
    int n = 10,i;
    //float val;
    REAL x[10]= {1,2,3,4,5,6,7,8,9,10};
    REAL y[10];
    //val=offset;

    for (i=0;i<=9;i++){  // collecting the data sets
        if (sysOff==true) return 0.0;
        if (direction==true)
            val+=0.000015;
        else
            val-=0.000015;
        if (val<0) val=0;
        if (val>0.0015) val=0.0015;
        servo.pulsewidth(0.0005 + val); // execute servo command
        wait(0.2); // wait for command done
        y[i]=(voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read())/10;
        //pc.printf("y=%g i=%g val=%g\r\n",y[i],i,val);
        pc.printf("val: %f    offset: %f\r\n",val,offset);

    }

    REAL m,b,r;
    linreg(n,x,y,&m,&b,&r);
    //pc.printf("m=%g b=%g r=%g\r\n",m,b,r);
    pc.printf("slope = %g\r\n",m);
    return m;
    }




float search2(bool direction,float val)   // search for vertical direction
    {
    int n = 10,i;
    //float val;
    REAL x[10]= {1,2,3,4,5,6,7,8,9,10};
    REAL y[10];
    //val=offset2;

    for (i=0;i<=9;i++){  // collecting the data sets
        //temp=(float) i;
        if (sysOff==true) return 0.0;
        if (direction==true)
            val+=0.000015; // turn either right or left
        else
            val-=0.000015;
        if (val<0) val=0;
        if (val>0.0015) val=0.0015;
        servo2.pulsewidth(0.0005 + val); // execute servo command
        wait(0.2); // wait for command done
        y[i]=(voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read())/10;
        //pc.printf("y=%g i=%g val=%g\r\n",y[i],i,val);
        pc.printf("val: %f    offset2: %f\r\n",val,offset2);
        
    }

    REAL m,b,r;
    linreg(n,x,y,&m,&b,&r);
    //pc.printf("m=%g b=%g r=%g\r\n",m,b,r);
    pc.printf("slope = %g\r\n",m);
    return m;
}










int main() {
    
    device.attach(&callback);  // serial communication interrupt
    init_servo();
    

    while (1) {
        //servo.pulsewidth(0.0015);
        //wait(0.25);


        if (sysOff==false){ // disabled by the server command
        currentVoltage=(voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read()+voltage.read())/10;
        
        
        val1=search(true,offset);   // search for horizontal max
        val2=search(false,offset);  // search for horizontal max, another direcion
        
        if ((val1>0.00005)&&(val2<-0.00005)) offset+=0.00005;  // check for slope to turn right
        if ((val2>0.00005)&&(val1<-0.00005)) offset-=0.00005;  // check for slope to turn left
        if ((val1>0.0001)&&(val2>0.0001))  {if (val1>val2) offset+=0.00005;else offset-=0.00005;}   // if could turn either way, turn with larger slope
        if (offset<0) offset=0;            // make sure not exceed range
        if (offset>0.0015) offset=0.0015;  // make sure not exceed range
        
        servo.pulsewidth(0.0005 + offset);  // execute servo command
        
         
        
        val1=search2(true,offset2);   // search for vertical max
        val2=search2(false,offset2);  // search for vertical max, another direction
        
        if ((val1>0.00005)&&(val2<-0.00005)) offset2+=0.00005;  // check for slope to turn up
        if ((val2>0.00005)&&(val1<-0.00005)) offset2-=0.00005;  // check for slope to turn down      
        if ((val1>0.0001)&&(val2>0.0001))  {if (val1>val2) offset2+=0.00005;else offset2-=0.00005;}  // if could turn either way, turn with larger slope
        if (offset2<0) offset2=0;            // make sure not exceed range
        if (offset2>0.0015) offset2=0.0015;  // make sure not exceed range
        
        servo2.pulsewidth(0.0005 + offset2);  // execute servo command
        
        servo3.pulsewidth(0.0005 + offset);  // execute servo command
        servo4.pulsewidth(0.0005 + offset2);  // execute servo command
        
        
        pc.printf("percentage: %3.3f% V    offset: %f    offset2: %f\r\n", currentVoltage*3.3*3,offset,offset2);
        wait(0.5);
        servo3.pulsewidth(0);servo4.pulsewidth(0);  // turn off servo so not vibrate
        if ((abs(offsetOld-offset)<0.000001)&&(abs(offset2Old-offset2)<0.000001)) {servo.pulsewidth(0);servo2.pulsewidth(0);servo3.pulsewidth(0);servo4.pulsewidth(0);wait(60.0);}
        offsetOld=offset;  // record data
        offset2Old=offset2;// record data
        
        }
    }
}

