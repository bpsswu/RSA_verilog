#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <sys/time.h>

uint64_t RL_binary(uint64_t base, uint64_t exp, uint64_t N);
int get_length(uint64_t input);
bool masking(uint64_t input, int n);
uint64_t long_div(uint64_t Msg, uint64_t N, int len);
uint64_t mod_exp(uint64_t A, uint64_t B, uint64_t N, int len);
uint64_t MM(uint64_t A, uint64_t B, uint64_t N, int len);
long getMicrotime();

int main()
{
    uint64_t M, N, e, d, C;
    N = 128255609;
    e = 17;
    d = 75431153;
    M = 52525252;
	
	long start_time = 0;
	long end_time	= 0;

    printf("Input message = %lu\n", M);
    
	start_time = getMicrotime();
    C = RL_binary(M, e, N);
	end_time = getMicrotime();

    printf("Cipher_text   = %lu\n", C);
	printf("encryption time = %ldus\n", end_time - start_time);
    
	start_time = getMicrotime();
    M = RL_binary(C, d, N);
	end_time = getMicrotime();

    printf("Plain_text    = %lu\n", M);
	printf("decryption time = %ldus\n", end_time - start_time);

    return 0;
}

uint64_t RL_binary(uint64_t base, uint64_t exp, uint64_t N)
{
    uint64_t r = 1;
    uint64_t y = base;
    int N_length = get_length(N);
    for(int i = 0; i < 32; i++) {
        if(masking(exp,i)) {
            r = mod_exp(r, y, N, N_length);
        }
        y = mod_exp(y, y, N, N_length);
    }
    return r;
}

int get_length(uint64_t input)
{
    int MSB_index = -1;
    if (input & 1) {
        MSB_index = 0;
    }
    for (int i = 1; i < 64; i++) {
        input = input >> 1;
        if (input & 1) {
            MSB_index = i;
        }
    }

    return MSB_index + 1;
}

bool masking(uint64_t input, int n)
{
    input = input >> n;
    input = input & 1;
    return input;
}

// len = length of N
uint64_t long_div(uint64_t Msg, uint64_t N, int len)
{    
    // multiply를 N으로 mod 한 결과값을 구하는 알고리즘
    uint64_t multiply;
    uint64_t R = pow(2, len);
    multiply = Msg * R;
    int Mlength = get_length(multiply);
    uint64_t buf1 = 0;
    uint64_t buf2 = 0;

    for (int i = Mlength; i > 0; i--) 
	{ //len+15 = multiply의 길이 = Mlength
        buf1 = buf1 << 1;
        if (masking(multiply, i - 1)) 
		{
            buf1++;
        }
        if (buf1 >= N) 
		{
            buf1 = buf1 - N;
            buf2 += 1;
            buf2 = buf2 << 1;
        }
    }
    return buf1;
}

uint64_t mod_exp(uint64_t A, uint64_t B, uint64_t N, int len) 
{
    // uint64_t R = pow(2, len);
	/*
	uint64_t MF = MM(R-N, R-N, N, len);

	uint64_t Z1 = MM(A, MF*MF, N, len);
	uint64_t Z2 = MM(B, MF*MF, N, len);
	uint64_t Z3 = MM(Z1, Z2, N, len);
	uint64_t Z4 = MM(Z3, 1, N, len);
    
	return Z4;
	*/
	/*
	uint64_t Z1 = MM(A, B, N, len);
	return MM(Z1, R*R, N, len);
	*/
	
	uint64_t Z1 = long_div(A, N, len);
    uint64_t Z2 = long_div(B, N, len);
    uint64_t Z3 = MM(Z1, Z2, N, len);
    uint64_t Z4 = MM(Z3, 1, N, len);
    
	return Z4;
}

uint64_t MM(uint64_t A, uint64_t B, uint64_t N, int len)
{
    uint64_t Z = 0;
    for(int i = 0; i < len; i++)
	{
        Z += masking(A, i) * B;
        Z += masking(Z, 0) * N;
        Z = Z >> 1;
    }
    while (Z > N)
	{
        Z -= N; 
    }
    return Z;
}

long getMicrotime(){
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}
