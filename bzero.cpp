#include <stdio.h>
#include <string.h>
#include <strings.h>

int main(void)
{
	char p_strText1[20]={0,};
	char p_strText2[20]={0,};

	for(int i=0; i<20; i++)
	{
		p_strText1[i]='a'+i;
		p_strText2[i]='A'+i;
	}

	printf("Text1: %s\n", p_strText1);
	printf("Text2: %s\n", p_strText2);

	printf("calling bzero() and memset() functions.\n");
	bzero(p_strText1, 20);
	memset(p_strText2, 0, 20);

	printf("text1: %s\n", p_strText1);
	printf("text2: %s\n", p_strText2);

	return 0;
}
