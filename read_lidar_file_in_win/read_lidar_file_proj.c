#include <stdio.h>

#include <stdlib.h>
    

#define MAXLEN 1024
#define IMODE 1
#define FIFO_SIZE           8192
#ifdef IMODE
#define DATA_WORDS	11
#else
#define DATA_WORDS	5
#endif

#ifdef IMODE
typedef	struct{
	int ch1;
	int ch2;
	int ch3;
	int ch4;
	int ch5;
	int ch6;
	int ch7;
	int ch8;
	unsigned int gps1;
	unsigned int gps2;
	unsigned int triTimes;
}Lidar_Data;
#else
typedef	struct{
	int ch1;
	int ch2;
	unsigned int gps1;
	unsigned int gps2;
	unsigned int triTimes;
} Lidar_Data;
#endif
typedef	struct{
	unsigned int timeData1;
	unsigned int timeData2;
	int year;
	int month;
	int day;
	int hour;//When use Beijing time, it should plus 8 hour.
	int minute;
	int second;
	int millisec;
	int microsec;
} Time_Data;

Lidar_Data lidarData[FIFO_SIZE] = {{0}};
Time_Data gpsData[FIFO_SIZE] = {{0}};

void FIFO_printData(Lidar_Data *data, int range, int step);
void Lidar_printData(Lidar_Data *data, Time_Data *gps, int range, int step);
void GPS_timeDataDecode(Time_Data *timeData);
    
int main(int argc, char *argv[]){
    if( argc < 3 )
    {
        printf("usage: %s %s/n", argv[0], "infile outfile");
        exit(1);
    }

    FILE * outfile, *infile;
    
    outfile = fopen(argv[2], "wb" );
	infile = fopen(argv[1], "rb");
    
   // unsigned char buf[MAXLEN];
    
    if( outfile == NULL || infile == NULL ){
        printf("%s, %s",argv[1],"not exit/n");
        exit(1);
    }   
    
    int rc;
    
/*    while( (rc = fread(buf,sizeof(unsigned char), MAXLEN,infile)) != 0 ){
        fwrite( buf, sizeof( unsigned char ), rc, outfile );
    }*/
    fread(lidarData, sizeof(int), DATA_WORDS*FIFO_SIZE, infile);

    FIFO_printData(lidarData, FIFO_SIZE, 300);

    for(i = 0; i< FIFO_SIZE, i++){
       gpsData[i].timeData1 = lidarData[i].gps1;
       gpsData[i].timeData2 = lidarData[i].gps2;
       GPS_timeDataDecode(&gpsData[i]);
    }

    fclose(infile);

    fclose(outfile);

    system("PAUSE");

    return 0;
}

void FIFO_printData(Lidar_Data *data, int range, int step){
    int i;
    #ifdef IMODE
    for(i = 0; i < range; i+= step){
        printf("%d\tgps1:%d\tgps2:%d\tch1:%d\tch2:%d\tch3:%d\tch4:%d\tch5:%d\tch6:%d\tch7:%d\tch8:%d\r\n",
        		(&data[i])->triTimes, (&data[i])->gps1, (&data[i])->gps2,
				(&data[i])->ch1, (&data[i])->ch2, (&data[i])->ch3,
				(&data[i])->ch4, (&data[i])->ch5, (&data[i])->ch6,
				(&data[i])->ch7, (&data[i])->ch8);
    }
    #else
    for(i = 0; i < range; i+= step){
        printf("%d\tgps1:%d\tgps2:%d\tch1:%d\tch2:%d\r\n",
        		(&data[i])->triTimes, (&data[i])->gps1, (&data[i])->gps2,
        		(&data[i])->ch1, (&data[i])->ch2);
    }
    #endif
}

void GPS_timeDataDecode(Time_Data *timeData){
	timeData->year = ((timeData->timeData1) >>19)&0xfff;
	timeData->month = ((timeData->timeData1) >> 15)& 0xf;
	timeData->day = ((timeData->timeData1) >> 10)& 0x1f;
	timeData->hour = ((timeData->timeData2)>> 22 )& 0x1f;
	timeData->minute = ((timeData->timeData2) >> 16 )& 0x3f;
	timeData->second = ((timeData->timeData2) >> 10 )& 0x3F;
	timeData->millisec = (timeData->timeData1) & 0x3ff;
	timeData->microsec = (timeData->timeData2) & 0x3ff;
	return;
}

void Lidar_printData(Lidar_Data *data, Time_Data *gps, int range, int step){
    int i;
    #ifdef IMODE
    for(i = 0; i < range; i+= step){
        printf("triTimes:%d\t Time:%4d/%2d/%2d %2d:%2d:%2d\tch1:%d\tch2:%d\tch3:%d\tch4:%d\tch5:%d\tch6:%d\tch7:%d\tch8:%d\r\n",
        		(&data[i])->triTimes, 
                (&gps[i])->year, (&gps[i])->month, (&gps[i])->day,
                (&gps[i])->hour, (&gps[i])->minute, (&gps[i])->second,
				(&data[i])->ch1, (&data[i])->ch2, (&data[i])->ch3,
				(&data[i])->ch4, (&data[i])->ch5, (&data[i])->ch6,
				(&data[i])->ch7, (&data[i])->ch8);
    }
    #else
    for(i = 0; i < range; i+= step){
        printf("triTimes:%d\t Time:%4d/%2d/%2d %2d:%2d:%2d\tch1:%d\tch2:%d\tch3:%d\tch4:%d\tch5:%d\tch6:%d\tch7:%d\tch8:%d\r\n",
        		(&data[i])->triTimes, 
                (&gps[i])->year, (&gps[i])->month, (&gps[i])->day,
                (&gps[i])->hour, (&gps[i])->minute, (&gps[i])->second,
        		(&data[i])->ch1, (&data[i])->ch2);
    }
    #endif
}
