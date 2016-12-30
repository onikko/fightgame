#pragma once

#define INPUT1 0x1
#define INPUT2 0x2
#define INPUT3 0x4
#define INPUT4 0x8
#define INPUT5 0x10
#define INPUT6 0x20
#define INPUT7 0x40
#define INPUT8 0x80
#define INPUT9 0x100
#define INPUT_P 0x200
#define INPUT_K 0x400
#define INPUT_G 0x800

#define INPUT_NOT1 0x1000
#define INPUT_NOT2 0x2000
#define INPUT_NOT3 0x4000
#define INPUT_NOT4 0x8000
#define INPUT_NOT5 0x10000
#define INPUT_NOT6 0x20000
#define INPUT_NOT7 0x40000
#define INPUT_NOT8 0x80000
#define INPUT_NOT9 0x100000
#define INPUT_NOT_G 0x200000
#define INPUT_NOT_P 0x400000
#define INPUT_NOT_K 0x800000

#define INPUT_NULL 0x40000000
#define INPUT_END 0x80000000

static const int
CMD1[] = { INPUT1, INPUT_END | 1 },		//��������
CMD2[] = { INPUT2, INPUT_END | 1 },		//������
CMD3[] = { INPUT3, INPUT_END | 1 },		//�E������
CMD4[] = { INPUT4, INPUT_END | 1 },		//������
CMD5[] = { INPUT5, INPUT_END | 1 },		//N����
CMD6[] = { INPUT6, INPUT_END | 1 },		//�E����
CMD7[] = { INPUT7, INPUT_END | 1 },		//�������
CMD8[] = { INPUT8, INPUT_END | 1 },		//�����
CMD9[] = { INPUT9, INPUT_END | 1 },		//�E�����
//CMD5P[] = { INPUT_NOT_P, INPUT_P, INPUT_END | 10 },	//�p���`����
//CMD5K[] = { INPUT_NOT_K, INPUT_K, INPUT_END | 10 },	//�L�b�N����
CMD5P[] = { INPUT_P, INPUT_END | 1 },	//�p���`����
CMD5K[] = { INPUT_K, INPUT_END | 1 },	//�L�b�N����
CMD3236P[] = { INPUT3, INPUT2, INPUT3, INPUT_NOT3 | INPUT6, INPUT_P, INPUT_END | 20 },
CMD8426P[] = { INPUT8, INPUT4, INPUT2, INPUT6, INPUT_NOT6 | INPUT_P, INPUT_END | 10 },
CMD6246P[] = { INPUT6, INPUT_NOT6, INPUT2, INPUT_NOT6, INPUT4, INPUT_NOT4, INPUT6, INPUT_NOT6, INPUT_P, INPUT_END | 20 };