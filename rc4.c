#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <openssl/rc4.h>

int main(int argc, char* argv[])
{
	int nFd_In = -1;	//파일기술자
	int nFd_Out = -1;	//암호화된거 기록할 파일기술자

	unsigned char p_cKey[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
	//암호화를 위한 비밀번호

	int nDataLen;	//읽어들일 데이터 길이
	unsigned char p_cInputData[BUFSIZ] = { 0 };		//읽어들일 데이터
	unsigned char p_cOutputData[BUFSIZ] = { 0 };	//기록될 데이터

	RC4_KEY stRc4Key;	//구조체 변수

	if (argc != 3)          
	{
		printf("usage: cmd <in-file> <out-file>\n");
		return -1;          //명령 3개 아니면 에러발생
	}

	printf("KEY=");	
	for (int i = 0; i < 16; i++) {	//키 출력(비밀번호)
		printf("%02X", p_cKey[i]);
	}
	printf("\n");

	nFd_In = open(argv[1], O_RDONLY);		//in 파일 기술자로 등록
	nFd_Out = open(argv[2], O_WRONLY | O_CREAT, 0644);	
	//out 파일기술자 등록_쓰기권한, 파일없으면 생성 후 0644 접근권한
	
	printf("set key.\n");
	RC4_set_key(&stRc4Key, 16, p_cKey);	//키 생성 -> 얘로 암호화, 복호화 함

	printf("Start to encrypt.\n");
	while (1) {
		nDataLen = read(nFd_In, p_cInputData, BUFSIZ);		//파일에서부터 읽어옴
		if (nDataLen <= 0)
			break;

		RC4(&stRc4Key, nDataLen, p_cInputData, p_cOutputData);	//암호화, 복호화 수행
			//(키정보, 길이, 읽어들인 데이터, 결과물 기록)

		if (write(nFd_Out, p_cOutputData, nDataLen) <= 0)	//결과를 파일에다가 기록
			break;
	}

	close(nFd_In);
	close(nFd_Out);

	printf("En/De-cryption is done.\n");

	return 0;
}
