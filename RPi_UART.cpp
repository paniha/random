#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>


#define GPIO_NUMBER "4"
#define GPIO4_PATH "/sys/class/gpio/gpio4/"
#define GPIO_SYSFS "/sys/class/gpio/"

void writeGPIO(char filename[], char value[]){
 FILE* fp; // create a file pointer fp
 fp = fopen(filename, "w"); // open file for write/update
 if(fp != NULL){
 fprintf(fp, "%s", value); // send the value to the file
 }else{
    printf("Can't open file\n");
 }
  fclose(fp); // close the file using fp
}

void openGPIO(){
    writeGPIO(GPIO_SYSFS "export", GPIO_NUMBER);
    usleep(100000); // sleep for 100ms
    writeGPIO(GPIO4_PATH "direction", "out");
}

void closeGPIO(){
 writeGPIO(GPIO_SYSFS "unexport", GPIO_NUMBER);
}

void setup(int uart0_filestream, int max ){
struct termios options;
tcgetattr (uart0_filestream, &options);
 options.c_cflag= B115200 | CS8 | CLOCAL | CREAD ;
options.c_iflag= IGNPAR;                                                              
options.c_lflag &=~(ICANON |ECHO|ECHOE|ISIG);// non-canonical
options.c_oflag= 0;   //rawdata
options.c_cc[VMIN]=max;  //wait for recievingmax bytes
options.c_cc[VTIME]=4;         //timeout in 2x 100ms
tcflush(uart0_filestream,TCIFLUSH);
tcsetattr(uart0_filestream, TCSANOW, &options);
}


int main(){
const char *device="/dev/ttyAMA0";  //;serial_clear(ss)–for uart on pin 8 and 10  it’s is TTYAMA0 unsigned 
char rx_buf[30];

char packet1[25]="Turning LED On \n";
char packet2[25]="Turning LED Off\n";
char packet3[25]="Terminating program\n";


memset(&rx_buf[0], 0, sizeof(rx_buf));
int n =0;
int fd;
fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY); 
printf("%i",fd);
if (fd == -1) {
std::cout << "no uart" << std::endl;
return (-1);
} 
else {
std::cout << " uart working\n" << std::endl;
setup(fd,25);
//initial the uart by parameters and set max 19 bytes for recieve buffer in noncanonical mode
}
fcntl(fd, F_SETFL, 0); //normal blocking mode re. Link on slide 12

//Loop for listening over UART
do{
n = read(fd, (void*) rx_buf, 3);
if(rx_buf[0] == '1'){
    printf("Turning on LED\n");
    openGPIO();
    writeGPIO(GPIO4_PATH "value", "1");
    closeGPIO();
    n = write(fd, packet1, strlen(packet1));
}else if(rx_buf[0] == '0'){
    printf("Turning off LED\n");
    openGPIO();
    writeGPIO(GPIO4_PATH "value", "0");
    closeGPIO();
    n = write(fd, packet2, strlen(packet2));
}
}while(rx_buf[0] != '3');
n = write(fd, packet3, strlen(packet3)); //Writes final message ove UART

close(fd);
}
