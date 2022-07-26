#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint8_t crc_table_8[256] = {0};
uint8_t crc_table_8_ref[256] = {0};
uint16_t crc_table_16[256] = {0};
uint16_t crc_table_16_ref[256] = {0};
uint32_t crc_table_32[256] = {0};
uint32_t crc_table_32_ref[256] = {0};
uint64_t crc_table_64[256] = {0};
uint64_t crc_table_64_ref[256] = {0};

// typedef uint8_t crc_t;
// static crc_t poly = 0x1e;//1edc6f41;
// uint8_t arr[] = {0xf5,0x94,0x26,0x1d,0x23, 0xf3,0x1d,0x78,0x00};//, 0xf5, 0x6f, 0x24, 0x1a, 0x32, 0x2d, 0x6a, 0x21};
// static crc_t seed = 0x12;
// static int w = 2;
// crc_t* crc_table = crc_table_8;
// crc_t* crc_table_ref = crc_table_8_ref;

// typedef uint16_t crc_t;
// static crc_t poly = 0x1edc;//1edc6f41;
// uint8_t arr[] = {0xf5,0x94,0x26,0x1d,0x23, 0xf3,0x1d,0x78,0x00};//, 0xf5, 0x6f, 0x24, 0x1a, 0x32, 0x2d, 0x6a, 0x21};
// static crc_t seed = 0x1234;
// static int w = 4;
// crc_t* crc_table = crc_table_16;
// crc_t* crc_table_ref = crc_table_16_ref;

typedef uint32_t crc_t;
static crc_t poly = 0x1edc6f41;
uint8_t arr[] = {0xf5,0x94,0x26,0x1d,0x23, 0xf3,0x1d,0x78,0x00};//, 0xf5, 0x6f, 0x24, 0x1a, 0x32, 0x2d, 0x6a, 0x21};
static crc_t seed = 0;
static int w = 8;
crc_t* crc_table = crc_table_32;
crc_t* crc_table_ref = crc_table_32_ref;

// typedef uint64_t crc_t;
// static crc_t poly = 0xad93d23594c93659;
// uint8_t arr[] = {0xf5,0x94,0x26,0x1d,0x23, 0xf3,0x1d,0x78,0x00};//, 0xf5, 0x6f, 0x24, 0x1a, 0x32, 0x2d, 0x6a, 0x21};
// static crc_t seed = 0;
// static int w = 16;
// crc_t* crc_table = crc_table_64;
// crc_t* crc_table_ref = crc_table_64_ref;


static uint ref_ip = 0;
static uint ref_seed = 0;
static uint ref_crc = 0;
static uint inv_seed = 0;
static uint inv_crc = 0;

static const int BIT_WIDTH = sizeof(crc_t) * 8;

uint8_t reflect8(uint8_t in)
{
	uint8_t out = 0;
	for( int i = 7; i >= 0; --i, in >>= 1 )
		out |= (in & 0x1) << i;
	return out;
}

uint16_t reflect16(uint16_t in)
{
	uint16_t out = 0;
	for( int i = 15; i >= 0; --i, in >>= 1 )
		out |= (in & 0x1) << i;
	return out;
}

uint32_t reflect32(uint32_t in)
{
	uint32_t out = 0;
	for( int i = 31; i >= 0; --i, in >>= 1 )
		out |= (in & 0x1) << i;
	return out;
}

crc_t reflect(crc_t in)
{
	crc_t out = 0;
	for( int i = BIT_WIDTH-1; i >= 0; --i, in >>= 1 )
		out |= (in & 0x1) << i;
	return out;
}

void print_table(crc_t* crc_table)
{
	const int ele_per_row = (BIT_WIDTH == 32 || BIT_WIDTH == 64) ? 8 : 16;
	for( int i = 0; i < 256/ele_per_row; i++ )
	{
		for(int j = 0; j < ele_per_row; j++ )
		{
			printf("0x%0*lX ",w, crc_table[i*ele_per_row + j]);
		}
		printf("\n");
	}
}

void generate_table()
{
	crc_t crc;
	for( int i = 0; i < 256; i++ ) {
		crc = (crc_t)i << BIT_WIDTH-8;
		for( int j = 8; j > 0 ; j--) {
			if( (crc & ((crc_t)1 << BIT_WIDTH-1)) != 0 ) {
				crc <<= 1;
				crc ^= poly;
			} else {
				crc <<= 1;
			}
		}
		crc_table[i] = crc;
	}
	// print_table(crc_table);
	// printf("\n");

	for( int i = 0; i < 256; i++ ) {
		crc = i;
		for( int j = 8; j > 0 ; j--) {
			if( crc & 1 != 0 ) {
				crc >>= 1;
				crc ^= reflect(poly);
			} else {
				crc >>= 1;
			}
		}
		crc_table_ref[i] = crc;
	}
	// print_table(crc_table_ref);
	// printf("\n");
}

uint8_t get_input(uint8_t in)
{
	if (ref_ip)
		return reflect8(in);
	return in;
}

crc_t get_seed(crc_t in)
{
	crc_t out = in;
	if (ref_seed)
		out = reflect(out);
	if (inv_seed)
		out = ~out;
	return out;
}

crc_t get_result(crc_t in)
{
	crc_t out = in;
	if (ref_crc)
		out = reflect(out);
	if (inv_crc)
		out = ~out;
	return out;
}

crc_t findcrc(const uint8_t input[], const crc_t seed, const size_t nb_elements)
{
	ref_ip = 0;
	ref_seed = 0;
	ref_crc = 0;
	inv_seed = 0;
	inv_crc = 0;

	crc_t crc = get_seed(seed);//input[0] << 24 | input[1] << 16 | input[2] << 8 | input[3];
	int bit_w = BIT_WIDTH;
	while( bit_w > 0 ) {
		bit_w -= 8;
		crc = crc ^ ((crc_t)get_input(*input) << bit_w);
		input++;
		// printf("crc = %08x\n",crc);
	}

	for( int b = 0; b < nb_elements; b++) {
		// printf("crc before %d iter = %0*lX\n", b, w,crc);
		for( int i = 8-1; i >= 0 ; i--)
		{
			if( (crc & ((crc_t)1 << BIT_WIDTH-1)) != 0 ) {
				crc <<= 1;
				crc |= (get_input(input[b]) & (1 << i)) ? 1 : 0;
				crc ^= poly;
			} else {
				crc <<= 1;
				crc |= (get_input(input[b]) & (1 << i)) ? 1 : 0;
			}
		}
		// printf("crc after %d iter = %0*lX\n", b, w,crc);
	}

	return get_result(crc);
}

crc_t findcrc_ref(const uint8_t input[], const crc_t seed, const size_t nb_elements)
{
	ref_ip = 1;
	ref_seed = 1;
	ref_crc = 1;
	inv_seed = 0;
	inv_crc = 0;

	crc_t crc = get_seed(seed);//input[3] << 24 | input[2] << 16 | input[1] << 8 | input[0];
	int bit_w = 0;
	while( bit_w < BIT_WIDTH ) {
		crc = crc ^ ((crc_t)get_input(*input) << bit_w);
		bit_w += 8;
		input++;
	}

	for( int b = 0; b < nb_elements; b++) {
		// printf("crc before %d iter = %0*lX\n", b, w,crc);
		for( int i = 0; i < 8 ; ++i)
		{
			if( (crc & 1) != 0 ) {
				crc >>= 1;
				crc |= (get_input(input[b]) & (1 << i)) ? (crc_t)1 << (BIT_WIDTH-1) : 0;
				crc ^= reflect(poly);
			} else {
				crc >>= 1;
				crc |= (get_input(input[b]) & (1 << i)) ? (crc_t)1 << (BIT_WIDTH-1) : 0;
			}
		}
		// printf("crc after %d iter = %0*lX\n", b, w,crc);
	}

	return get_result(crc);
}

crc_t findcrc2(const uint8_t input[], const crc_t seed, const size_t nb_elements)
{
	ref_ip = 0;
	ref_seed = 0;
	ref_crc = 0;
	inv_seed = 0;
	inv_crc = 0;

	crc_t crc = get_seed(seed);

	for( int b = 0; b < nb_elements; b++)
	{
		crc ^= (crc_t)get_input(input[b]) << (BIT_WIDTH - 8);
		// printf("crc before %d iter = %0*lX\n", b+1,w, crc);

		for( int i = 1; i <= 8 ; i++)
		{
			if( (crc & ((crc_t)1 << BIT_WIDTH-1)) != 0 ) {
				crc <<= 1;
				crc ^= poly;
			} else {
				crc <<= 1;
			}
		}
		// printf("crc after %d iter = %0*lX\n", b+1,w, crc);
	}

	return get_result(crc);
}

crc_t findcrc2_ref(const uint8_t input[], const crc_t seed, const size_t nb_elements)
{
	ref_ip = 1;
	ref_seed = 1;
	ref_crc = 1;
	inv_seed = 0;
	inv_crc = 0;

	crc_t crc = get_seed(seed);

	for( int b = 0; b < nb_elements; b++)
	{
		crc ^= get_input(input[b]);
		// printf("crc before %d iter = %0*lX\n", b+1,w, crc);

		for( int i = 1; i <= 8 ; i++)
		{
			if( (crc & 1) != 0 ) {
				crc >>= 1;
				crc ^= reflect(poly);
			} else {
				crc >>= 1;
			}
		}
		// printf("crc after %d iter = %0*lX\n", b+1,w, crc);
	}

	return get_result(crc);
}

crc_t findcrc_table(const uint8_t input[], const crc_t seed, const size_t nb_elements, crc_t* table)
{
	ref_ip = 0;
	ref_seed = 0;
	ref_crc = 0;
	inv_seed = 0;
	inv_crc = 0;

	crc_t crc = get_seed(seed);

	for( int b = 0; b < nb_elements; b++)
	{
		uint8_t pos = (crc >> (BIT_WIDTH - 8)) ^ get_input(input[b]);
		crc = (crc << 8) ^ table[pos];
		// printf("crc after %d iter = %0*lX\n", b,w, crc);
	}

	return get_result(crc);
}

crc_t findcrc_table_ref(const uint8_t input[], const crc_t seed, const size_t nb_elements, crc_t* table)
{
	ref_ip = 1;
	ref_seed = 1;
	ref_crc = 1;
	inv_seed = 0;
	inv_crc = 0;

	crc_t crc = get_seed(seed);

	for( int b = 0; b < nb_elements; b++)
	{
		uint8_t pos = (crc ^ get_input(input[b])) & 0xff;
		crc = (crc >> 8) ^ table[pos];
		// printf("crc after %d iter = %0*lX\n", b,w, crc);
	}

	return get_result(crc);
}


int main(int argc, char** argv)
{
	generate_table();

	crc_t mycrc = findcrc(arr, seed, (sizeof(arr)/sizeof(arr[0]))-1);
	printf("crc from crc  = %0*lX\n", w,mycrc);
	crc_t mycrc_ref = findcrc_ref(arr, seed, (sizeof(arr)/sizeof(arr[0]))-1);
	printf("crc from crc ref  = %0*lX\n", w,mycrc_ref);
	crc_t mycrc2 = findcrc2(arr, seed, (sizeof(arr)/sizeof(arr[0]))-1);
	printf("crc from crc2 = %0*lX\n",w, mycrc2);
	crc_t mycrc2_ref = findcrc2_ref(arr, seed, (sizeof(arr)/sizeof(arr[0]))-1);
	printf("crc from crc2 ref = %0*lX\n",w, mycrc2_ref);
	crc_t mycrc3 = findcrc_table(arr, seed, (sizeof(arr)/sizeof(arr[0]))-1, crc_table);
	printf("crc from table = %0*lX\n",w, mycrc3);
	crc_t mycrc3_ref = findcrc_table_ref(arr, seed, (sizeof(arr)/sizeof(arr[0]))-1, crc_table_ref);
	printf("crc from table ref = %0*lX\n",w, mycrc3_ref);

	return 0;
}