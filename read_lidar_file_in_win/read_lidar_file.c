#include <stdio.h>
#include <stdlib.h>
#include "io.h"
#include "read_lidar_file.h"

Lidar_Data lidarData[READ_SIZE] = {{0}};
Time_Data gpsData[READ_SIZE] = {{0}};
unsigned char buf[DATA_LEN * READ_SIZE];

int Lidar_TransferDataFile(char *in_file_path, char *out_file_path){
    FILE *outfile, *infile;
    unsigned int read_words = 0;
    unsigned int line_num = 0;
    unsigned int write_num = 0;
    
    outfile = fopen(out_file_path, "wb" );
	infile = fopen(in_file_path, "rb");
    
    read_words = fread(lidarData, sizeof(int), DATA_WORDS * READ_SIZE, infile);

    line_num = read_words / DATA_WORDS;
    Lidar_dataDecode(lidarData, gpsData, line_num);

    write_num = Lidar_printDataToString(lidarData, gpsData, buf, line_num);
    fwrite(buf, sizeof(unsigned char), write_num, outfile);

    fclose(infile);
    fclose(outfile);
    return 0;
}

void Lidar_dataDecode(Lidar_Data *data, Time_Data *time, unsigned int decode_num){
	int i;
	for (i = 0; i < decode_num; i++){
       gpsData[i].timeData1 = lidarData[i].gps1;
       gpsData[i].timeData2 = lidarData[i].gps2;
       GPS_timeDataDecode(&gpsData[i]);
    }
}

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

unsigned int Lidar_printDataToString(Lidar_Data *data, Time_Data *gps, char *buf, unsigned int line_num){
    int line_len = 0;
	int title_len = 0;
    int totle_print_num = 0;

	line_len = strlen(DATA_FORMAT) + 12;//12 is a test result, if it less than 12, data would be writen in one line.
	title_len = strlen(TITLE_FORMAT);

    int i;
    sprintf(buf, TITLE_FORMAT);
    #ifdef IMODE
    for(i = 0; i < line_num; i++){
        sprintf(buf + title_len + i*(line_len), DATA_FORMAT,
                (&gps[i])->year, (&gps[i])->month, (&gps[i])->day,
                (&gps[i])->hour, (&gps[i])->minute, (&gps[i])->second,
                (&gps[i])->millisec, (&gps[i])->microsec,
				(&data[i])->ch1, (&data[i])->ch2, (&data[i])->ch3,
				(&data[i])->ch4, (&data[i])->ch5, (&data[i])->ch6,
				(&data[i])->ch7, (&data[i])->ch8);
    }
    #else
    for(i = 0; i < line_num; i++){
		sprintf(buf + title_len + i*(line_len), DATA_FORMAT,
                (&gps[i])->year, (&gps[i])->month, (&gps[i])->day,
                (&gps[i])->hour, (&gps[i])->minute, (&gps[i])->second,
                (&gps[i])->millisec, (&gps[i])->microsec,
        		(&data[i])->ch1, (&data[i])->ch2,
                (&data[i])->delay1, (&data[i])->delay2, 
                (&data[i])->delay3, (&data[i])->delay4);
    }
    #endif
    return totle_print_num = title_len + line_len * line_num;
}


int Lidar_TransferAllDataFilesInDir(char *dir_path){
    struct _finddata_t files;
    int File_Handle;
	int file_num = 0;
    char file_check_path[MAXLEN] = "0";
    char out_file_path[MAXLEN] = "0";
    char in_file_path[MAXLEN] = "0";

    Lidar_GetDataFileCheckPath(file_check_path, dir_path);
    File_Handle = _findfirst(file_check_path,&files);
    if(File_Handle == -1){
        printf("Please check the dir path and ensure there are data files in the dir!\n");
        return 0;
    }
    do{
        Lidar_GetFilePath(in_file_path, out_file_path, dir_path, files.name);
        Lidar_TransferDataFile(in_file_path, out_file_path);
        printf("%s \n", out_file_path);
		file_num++;
    }while(0 == _findnext(File_Handle,&files));

    _findclose(File_Handle);

	printf("Transfered %d files\n", file_num);
    return 0;
}

int Lidar_GetFilePath(char *in_file_path, char *out_file_path, char *dir_path, char *file_name){
    Lidar_getInFilePath(in_file_path, dir_path, file_name);
    Lidar_getOutFilePath(out_file_path, dir_path, file_name);
    return 0;
}

int Lidar_GetDataFileCheckPath(char *file_check_path, char *dir_path){
    strcpy(file_check_path, dir_path);
    strcat(file_check_path, ORIGINAL_FILE_NAME);
    return 0;
}
