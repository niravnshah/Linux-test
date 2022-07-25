#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// typedef uint8_t crc_t;
// static const crc_t poly = 0x1e;//1edc6f41;
// crc_t arr[] = {0xf5,0x94,0x26,0x1d,0x23, 0xf3,0x1d,0x78,0x0000};//, 0xf5, 0x6f, 0x24, 0x1a, 0x32, 0x2d, 0x6a, 0x21};
// static int w = 2;

// typedef uint16_t crc_t;
// static const crc_t poly = 0x1edc;//1edc6f41;
// crc_t arr[] = {0xf594,0x261d,0x23f3,0x1d78,0x0000};//, 0xf5, 0x6f, 0x24, 0x1a, 0x32, 0x2d, 0x6a, 0x21};
// static int w = 4;

typedef uint32_t crc_t;
static const crc_t poly = 0x1edc6f41;//1edc6f41;
crc_t arr[] = {0xf594261d,0x23f31d78,0x0000};//, 0xf5, 0x6f, 0x24, 0x1a, 0x32, 0x2d, 0x6a, 0x21};
static int w = 8;



static const int BIT_WIDTH = sizeof(crc_t) * 8;


crc_t crc_table[256] = {0};

void generate_table()
{
	crc_t crc;
	for( int i = 0; i < 256; i++ )
	{
		crc = i;
		for( int i = BIT_WIDTH-1; i >= 0 ; i--)
		{
			if( (crc & (1 << BIT_WIDTH-1)) != 0 ) {
				crc <<= 1;
				crc ^= poly;
			} else {
				crc <<= 1;
			}
		}
		crc_table[i] = crc;
	}
}

void print_table()
{
	const int ele_per_row = 8;
	for( int i = 0; i < 256/ele_per_row; i++ )
	{
		for(int j = 0; j < ele_per_row; j++ )
		{
			printf("0x%0*X ",w, crc_table[i*ele_per_row + j]);
		}
		printf("\n");
	}
}

crc_t findcrc(const crc_t input[], const size_t nb_elements)
{
	crc_t crc = input[0];

	for( int b = 1; b <= nb_elements; b++) {
		// printf("crc before %d iter = %0*X\n", b, w,crc);
		for( int i = BIT_WIDTH-1; i >= 0 ; i--)
		{
			if( (crc & (1 << BIT_WIDTH-1)) != 0 ) {
				crc <<= 1;
				crc |= (input[b] & (1 << i)) ? 1 : 0;
				crc ^= poly;
			} else {
				crc <<= 1;
				crc |= (input[b] & (1 << i)) ? 1 : 0;
			}
		}
		// printf("crc after %d iter = %0*X\n", b, w,crc);
	}

	return crc;
}

crc_t findcrc2(const crc_t input[], const size_t nb_elements)
{
	crc_t crc = 0;

	for( int b = 0; b < nb_elements; b++)
	{
		crc ^= input[b];
		// printf("crc before %d iter = %0*X\n", b,w, crc);

		for( int i = 0; i < BIT_WIDTH ; i++)
		{
			if( (crc & (1 << BIT_WIDTH-1)) != 0 ) {
				crc <<= 1;
				crc ^= poly;
			} else {
				crc <<= 1;
			}
		}
		// printf("crc after %d iter = %0*X\n", b,w, crc);
	}

	return crc;
}

crc_t findcrc_table(const crc_t input[], const size_t nb_elements)
{
	crc_t crc = 0;

	for( int b = 0; b < nb_elements; b++)
	{
		crc = crc_table[crc ^ input[b]];
	}

	return crc;
}

int main(int argc, char** argv)
{
	crc_t mycrc = findcrc(arr, (sizeof(arr)/sizeof(arr[0]))-1);
	printf("crc from crc  = %0*x\n", w,mycrc);
	crc_t mycrc2 = findcrc2(arr, (sizeof(arr)/sizeof(arr[0]))-1);
	printf("crc from crc2 = %0*x\n",w, mycrc2);
	generate_table();
	//print_table();

	return 0;
}