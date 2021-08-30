#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <openssl/aes.h>
#include <getopt.h>	//-e,-d 받기위함

int main(int argc, char* argv[])
{
	int nFd_In = -1;	//파일기술자
	int nFd_Out = -1;	//암호화된거 기록할 파일기술자
	int nRead, nWritten;	//기록, 읽기위한 길이설정
	unsigned char p_cInputData[AES_BLOCK_SIZE];	//내용 읽기위한 변수
	unsigned char p_cOutputData[AES_BLOCK_SIZE];	//내용 정할 변수
	int bEncryption = false;	//암호화, 복호화 결정할 변수
	int nOpt = -1;	//명령에서 옵션값 받기위한 변수

	unsigned char p_ckey[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };	//키 값
	unsigned char p_cIvec[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 }; //백터 값

	AES_KEY stKey;	//구조체 변수

	if (argc != 4)          
	{
		printf("usage: %s <-e/-d> <in-file> <out-file>\n", argv[0]);
		return -1;          //명령 4개 아니면 에러발생
	}
	
	while ((nOpt=getopt(argc, argv, "ed"))>0) {	//명령 구분
		switch (nOpt) {
			case 'e':
				bEncryption = true;		//암호화함
				break;
			case 'd':
				bEncryption = false;	//복호화함
				break;
			default:
				printf("usage: %s <-e/-d> <in-file> <out-file>\n");
				return -1;
		}
	}

	printf("CKEY=");
	for (int i = 0; i < 16; i++) {		//키값 출력
		printf("%02X", p_ckey[i]);
	}
	printf("\n");

	printf("IVEC=");
	for (int i = 0; i < 16; i++) {		//벡터값 출력
		printf("%02X", p_cIvec[i]);
	}
	printf("\n");

	printf("Input File: %s\n", argv[optind]);		//입력파일 출력
	printf("Output File: %s\n", argv[optind + 1]);		//출력파일 출력

	nFd_In = open(argv[optind], O_RDONLY);		//입력파일 접근
	nFd_Out = open(argv[optind + 1], O_WRONLY | O_CREAT, 0644);		//출력파일 접근

	AES_set_encrypt_key(p_ckey, 128, &stKey);	//키 생성, 128비트 암호화 수행, stkey값으로 설정

	int nBlockIndex = 0;
	while (1) {
		nRead = read(nFd_In, p_cInputData, AES_BLOCK_SIZE);	
		//AES_BLOCK_SIZE 메크로로 정의, 길이만큼 수행
		if (nRead <= 0)
			break;

		if (bEncryption) {	//true일때 암호화
			AES_cfb128_encrypt(p_cInputData, p_cOutputData, nRead,
								&stKey, p_cIvec, &nBlockIndex,
				//다음 블록(체인)을 위해 값 갱신_각각 위치 기록위함
								AES_ENCRYPT);	//메크로로 1
			//(지정된파일에서 키값, 벡터값 사용해서 output에다가 기록)
		}
		else {		//false일때 복호화
			AES_cfb128_encrypt(p_cInputData, p_cOutputData, nRead,
								&stKey, p_cIvec, &nBlockIndex,
								AES_DECRYPT);		//메크로로 0
		}

		nWritten = write(nFd_Out, p_cOutputData, nRead);
		//출력파일에 기록
	}

	close(nFd_In);
	close(nFd_Out);

	return 0;
}
