#include <stdio.h>
#include <stdlib.h>

/************** Choose Mode *************/
#define IMODE 1
/****************************************/

#define MAXLEN 1024
#define FIFO_SIZE           8192
#define FIFO_NUM_EACH_PAGE  100
#define READ_SIZE   FIFO_SIZE * FIFO_NUM_EACH_PAGE

#ifdef IMODE
#define DATA_WORDS	11
#define DATA_LEN    100
#else
#define DATA_WORDS	5
#define DATA_LEN    50 //need to be test.
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

Lidar_Data lidarData[READ_SIZE] = {{0}};
Time_Data gpsData[READ_SIZE] = {{0}};
unsigned char buf[DATA_LEN * READ_SIZE];

void FIFO_printData(Lidar_Data *data, int range, int step);
void Lidar_printData(Lidar_Data *data, Time_Data *gps, int range, int step);
void Lidar_printDataToString(Lidar_Data *data, Time_Data *gps, char *buf);
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
    
    if( outfile == NULL || infile == NULL ){
        printf("%s, %s",argv[1],"not exit/n");
        exit(1);
    }   
    
    fread(lidarData, sizeof(int), DATA_WORDS * FIFO_SIZE * FIFO_NUM_EACH_PAGE, infile);

	int i;
	for (i = 0; i < FIFO_SIZE * FIFO_NUM_EACH_PAGE; i++){
       gpsData[i].timeData1 = lidarData[i].gps1;
       gpsData[i].timeData2 = lidarData[i].gps2;
       GPS_timeDataDecode(&gpsData[i]);
    }
 //   Lidar_printData(lidarData, gpsData FIFO_SIZE, 300);

    Lidar_printDataToString(lidarData, gpsData, buf);
    fwrite( buf, sizeof( unsigned char ), sizeof(buf), outfile );

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
}

void Lidar_printData(Lidar_Data *data, Time_Data *gps, int range, int step){
    int i;
    #ifdef IMODE
    for(i = 0; i < range; i+= step){
        printf("triTimes:%04d\t Time:%04d/%02d/%02d-%02d:%02d:%02d-%03d:%03d\tch1:%06d\tch2:%06d\tch3:%06d\tch4:%06d\tch5:%06d\tch6:%06d\tch7:%06d\tch8:%06d\r\n",
                (&gps[i])->year, (&gps[i])->month, (&gps[i])->day,
                (&gps[i])->hour, (&gps[i])->minute, (&gps[i])->second,
                (&gps[i])->millisec, (&gps[i])->microsec,
				(&data[i])->ch1, (&data[i])->ch2, (&data[i])->ch3,
				(&data[i])->ch4, (&data[i])->ch5, (&data[i])->ch6,
				(&data[i])->ch7, (&data[i])->ch8);
    }
    #else
    for(i = 0; i < range; i+= step){
        printf("triTimes:%04d\t Time:%04d/%02d/%02d-%02d:%02d:%02d-%03d:%03d\tch1:%08d\tch2:%08d\r\n)",
        		(&data[i])->triTimes, 
                (&gps[i])->year, (&gps[i])->month, (&gps[i])->day,
                (&gps[i])->hour, (&gps[i])->minute, (&gps[i])->second,
                (&gps[i])->millisec, (&gps[i])->microsec,
        		(&data[i])->ch1, (&data[i])->ch2);
    }
    #endif
}

#ifdef IMODE
#define DATA_FORMAT "%04d/%02d/%02d-%02d:%02d:%02d-%03d:%03d\t%6d\t%6d\t%6d\t%6d\t%6d\t%6d\t%6d\t%6d\n"
#define TITLE_FORMAT "time\tch1\tch2\tch3\tch4\tch5\tch6\tch7\tch8\n"
#else
#define DATA_FORMAT "%04d/%02d/%02d-%02d:%02d:%02d-%03d:%03d\tch1\t%8d\tch2\t%8d\n" 
#define TITLE_FORMAT "time\tch1\tch2\n"
#endif

void Lidar_printDataToString(Lidar_Data *data, Time_Data *gps, char *buf){
    int i;
    int line_len = 0;
	int title_len = 0;
	line_len = strlen(DATA_FORMAT) + 16;
	title_len = strlen(TITLE_FORMAT);

    sprintf(buf, TITLE_FORMAT);
    #ifdef IMODE
    for(i = 1; i < READ_SIZE; i++){
        sprintf(buf + title_len + i*(line_len), DATA_FORMAT,
                (&gps[i])->year, (&gps[i])->month, (&gps[i])->day,
                (&gps[i])->hour, (&gps[i])->minute, (&gps[i])->second,
                (&gps[i])->millisec, (&gps[i])->microsec,
				(&data[i])->ch1, (&data[i])->ch2, (&data[i])->ch3,
				(&data[i])->ch4, (&data[i])->ch5, (&data[i])->ch6,
				(&data[i])->ch7, (&data[i])->ch8);
    }
    #else
    for(i = 0; i < READ_SIZE; i++){
        sprintf(buf + i*(line_len + 4), DATA_FORMAT,
        		(&data[i])->triTimes, 
                (&gps[i])->year, (&gps[i])->month, (&gps[i])->day,
                (&gps[i])->hour, (&gps[i])->minute, (&gps[i])->second,
                (&gps[i])->millisec, (&gps[i])->microsec,
        		(&data[i])->ch1, (&data[i])->ch2);
    }
    #endif
}
