/************** Choose Mode *************/
#define IMODE 1
/****************************************/

#define MAXLEN 1024
#define FIFO_SIZE           8192
#define FIFO_NUM_EACH_PAGE  10
#define READ_SIZE   FIFO_SIZE * FIFO_NUM_EACH_PAGE
#define OUT_FILE_MARK  "/Transfered_"
#define ORIGINAL_FILE_NAME  "/data*.xls"

#ifdef IMODE
#define DATA_WORDS	11
#define DATA_LEN    85
#define DATA_FORMAT "%04d/%02d/%02d-%02d:%02d:%02d-%03d:%03d\t%6d\t%6d\t%6d\t%6d\t%6d\t%6d\t%6d\t%6d\n"
#define TITLE_FORMAT "time\tch1\tch2\tch3\tch4\tch5\tch6\tch7\tch8\n"
#else
#define DATA_WORDS	5
#define DATA_LEN    50 //need to be test.
#define DATA_FORMAT "%04d/%02d/%02d-%02d:%02d:%02d-%03d:%03d\tch1\t%8d\tch2\t%8d\n" 
#define TITLE_FORMAT "time\tch1\tch2\n"
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


void FIFO_printData(Lidar_Data *data, int range, int step);
void Lidar_printData(Lidar_Data *data, Time_Data *gps, int range, int step);
unsigned int Lidar_printDataToString(Lidar_Data *data, Time_Data *gps, char *buf, unsigned int line_num);
void Lidar_getOutFilePath(char *full_path, char *dir_path, char *file_name);
void Lidar_getInFilePath(char *full_path, char *dir_path, char *file_name);
void GPS_timeDataDecode(Time_Data *timeData);
int Lidar_TransferDataFile(char *in_file_path, char *out_file_path);
void Lidar_dataDecode(Lidar_Data *data, Time_Data *time, unsigned int decode_num);

int Lidar_TransgerAllDataFilesInDir(char *dir_path);
int Lidar_GetDataFileCheckPath(char *file_check_path, char *dir_path);
int Lidar_GetFilePath(char *in_file_path, char *out_file_path, char *dir_path, char *file_name);
