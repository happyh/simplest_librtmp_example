/**
 * Simplest Librtmp Send 264
 *
 * �����裬����
 * leixiaohua1020@126.com
 * zhanghuicuc@gmail.com
 * �й���ý��ѧ/���ֵ��Ӽ���
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * ���������ڽ��ڴ��е�H.264����������RTMP��ý���������
 * This program can send local h264 stream to net server as rtmp live stream.
 */

#include <stdio.h>
#include "librtmp_send264.h"



FILE *fp_send1;

//���ļ��Ļص�����
//we use this callback function to read data from buffer
int read_buffer1(unsigned char *buf, int buf_size ){
	if(!feof(fp_send1)){
		int true_size=fread(buf,1,buf_size,fp_send1);
		return true_size;
	}else{
		return -1;
	}
}

int main(int argc, char* argv[])
{
	if (1) {
		fp_send1 = fopen("cuc_ieschool.h264", "rb");

		//��ʼ�������ӵ�������
		RTMP264_Connect("rtmp://10.10.130.103/liveshow/happyhe");

		//����
		RTMP264_Send(read_buffer1);

		//�Ͽ����Ӳ��ͷ������Դ
		RTMP264_Close();
	}
	else {
		
		FILE *fp_264 = fopen("d:/2.h264", "wb");
		FILE *fp_dump = fopen("d:/2.raw", "rb");

		fseek(fp_dump, 7849, 0); // 2.raw: 7849 ,3.raw:6941

		RTMP264_Read(fp_dump, fp_264,4096);
	}
	return 0;
}

