#include <stdio.h>
#include <stdlib.h>
#include"io.h"

/************** Choose Mode *************/
#define IMODE 1
/****************************************/

#define MAXLEN 1024
#define FIFO_SIZE           8192
#define FIFO_NUM_EACH_PAGE  10
#define READ_SIZE   FIFO_SIZE * FIFO_NUM_EACH_PAGE

#ifdef IMODE
#define DATA_WORDS	11
#define DATA_LEN    85
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
void Lidar_getOutFilePath(char *full_path, char *dir_path, char *file_name);
void Lidar_getInFilePath(char *full_path, char *dir_path, char *file_name);
void GPS_timeDataDecode(Time_Data *timeData);
int Lidar_TransferDataFile(char *in_file_path, char *out_file_path);

#define ORIGINAL_FILE_NAME  "/data*.xls"
int main(int argc, char *argv[])
{
    struct _finddata_t files;
    int File_Handle;
    int i=0;
    char file_check_path[MAXLEN] = "0";
    char *dir_path;
    char out_file_path[MAXLEN] = "0";
    char in_file_path[MAXLEN] = "0";
    FILE *outfile, *infile;

    if( argc < 2 )
    {
        printf("usage: %s %s/n", argv[0], "dir_path");
        exit(1);
    }
    dir_path = argv[1];

    strcpy(file_check_path, dir_path);
    strcat(file_check_path, ORIGINAL_FILE_NAME);

    File_Handle = _findfirst(file_check_path,&files);
    if(File_Handle==-1)
    {
        printf("error\n");
        return 0;
    }
    do
    {
        printf("%s \n",files.name);
        Lidar_getInFilePath(in_file_path, dir_path, files.name);
        printf("%s \n", in_file_path);
        Lidar_getOutFilePath(out_file_path, dir_path, files.name);
        printf("%s \n", out_file_path);
        Lidar_TransferDataFile(in_file_path, out_file_path);
        i++;
    }while(0==_findnext(File_Handle,&files));
    _findclose(File_Handle);
    printf("Transfered %d files\n",i);
    system("PAUSE");
    return 0;
}

int Lidar_TransferDataFile(char *in_file_path, char *out_file_path){
    FILE *outfile, *infile;
    
    outfile = fopen(out_file_path, "wb" );
	infile = fopen(in_file_path, "rb");
    
    fread(lidarData, sizeof(int), DATA_WORDS * FIFO_SIZE * FIFO_NUM_EACH_PAGE, infile);

	int i;
	for (i = 0; i < READ_SIZE; i++){
       gpsData[i].timeData1 = lidarData[i].gps1;
       gpsData[i].timeData2 = lidarData[i].gps2;
       GPS_timeDataDecode(&gpsData[i]);
    }

    Lidar_printDataToString(lidarData, gpsData, buf);
    fwrite(buf, sizeof( unsigned char ), sizeof(buf), outfile);

    fclose(infile);
    fclose(outfile);

    return 0;
}

#define OUT_FILE_MARK  "/Transfered_"
void Lidar_getOutFilePath(char *full_path, char *dir_path, char *file_name){
    strcpy(full_path, dir_path);
    strcat(full_path, OUT_FILE_MARK);
    strcat(full_path, file_name); 
}

void Lidar_getInFilePath(char *full_path, char *dir_path, char *file_name){
    strcpy(full_path, dir_path);
    strcat(full_path, "/"); 
    strcat(full_path, file_name); 
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
	line_len = strlen(DATA_FORMAT) + 12;//12 is a test result, if it less than 12, data would be writen in one line.
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
		sprintf(buf + title_len + i*(line_len), DATA_FORMAT,
        		(&data[i])->triTimes, 
                (&gps[i])->year, (&gps[i])->month, (&gps[i])->day,
                (&gps[i])->hour, (&gps[i])->minute, (&gps[i])->second,
                (&gps[i])->millisec, (&gps[i])->microsec,
        		(&data[i])->ch1, (&data[i])->ch2);
    }
    #endif
}


