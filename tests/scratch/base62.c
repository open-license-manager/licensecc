

/* Division by 62 on an arbitrary length input.
   (TODO: Note: the cast will segfault on some systems, so you will need
   to use a memcpy instead).
   Also TODO: Optimisation: use >32 bit integers if available.
 */
static void b62_divide(const unsigned char* dividend, int dividend_len,
					   unsigned char* quotient, unsigned int* remainder)
{
	unsigned int quantity;
	int i;

	quantity = 0;
	for (i=dividend_len-2;i>=0;i-=2) {
		quantity |= *((unsigned short*)&dividend[i]);
		*((unsigned short *)&quotient[i]) = (unsigned short)(quantity/62);
		quantity = (quantity%62)<<16;
	}
	*remainder = quantity>>16;
}

/* pseudo-base62 encode
	base62 encoding is not a very nice mapping to character data.  The only
	way that we can properly do this is to divide by 62 each time and get
	the remainder, this will ensure full use of the base62 space.  This is
	/not/ very performant.  So instead we operate the base62 encoding on
	56 bit (7 byte) chunks.  This gives a pretty good usage, with far less
	lengthy division operations on moderately sized input.  I only did this
	for completeness as I got interested in it, but we can prove that you have
	to do the full division each time (although you may find a better way of
	implementing it!) as follows.  We want to encode data as a bitstream, so
	we want to find N,M s.t. 62^M = 2^N, and M,N are integers.  There are
	no such M,N (proof on request).  For base64 encoding we get 64^M = 2^N,
	obviously we can fit M=1,N=6 which equates to sucking up 6 bits each time
	for the encoding algorithm.  So instead we try to find a comprise between
	the the block size, and the number of bits wasted by the conversion to
	base62 space.  The constraints here are

	(1) we want to deal with whole numbers of bytes to simplify the code
	(2) we want to waste as little of the encoding space as possible
	(3) we want to keep the division operations restricted to a reasonable
	number of bits as the running time of the division operation depends
	on the length of the input bit string.

	The ratio of the length of the bit strings in the two bases will be
	log2(256)/log2(62)
	For base64 encoding we get 4/3 exactly.  So to minimize waste here we
	want to take chunks of 3 bytes, then there is no wastage between blocks.
	For base62 encoding we get ~1.34.  Picking 5 as the block size wastes
	some 30% of the encoding space for the last byte.  Let me know if you
	think another pick is better.  This one means that we are operating
	on 40-bit strings, so the division isn't too strenuous to compute, and
	on a 64-bit platform can be done all in a register.
 */
#define msg_id_length (((134359*(sizeof(time_t)+sizeof(int)*2))/100000)+2)

static char base62_tab[62] = {
	'A','B','C','D','E','F','G','H',
	'I','J','K','L','M','N','O','P',
	'Q','R','S','T','U','V','W','X',
	'Y','Z','a','b','c','d','e','f',
	'g','h','i','j','k','l','m','n',
	'o','p','q','r','s','t','u','v',
	'w','x','y','z','0','1','2','3',
	'4','5','6','7','8','9'
};

int b62_encode(char* out, const unsigned char* data, int length, int linelength)
{
	int i,j;
	char *start = out;
	uint64_t bitstring;

	linelength;
	for (i=0;i<length-4;i+=5) {
		bitstring =
			(uint64_t)data[i]<<32|(uint64_t)data[i+1]<<24|(uint64_t)data[i+2]<<16|
			(uint64_t)data[i+3]<<8|(uint64_t)data[i+4];

		for (j=0;j<7;++j) {
			*out++ = base62_tab[bitstring%62];
			bitstring /= 62;
		}
/*
		b62_divide(quotient,len,quotient,&rem);
		*out++ = base62_tab[rem];
		for (j=1;j<len;++j) {
			b62_divide(quotient,len,quotient,&rem);
			*out++ = base62_tab[rem];
		}*/
	}
	switch (length-i) {
	case 1:
		*out++ = base62_tab[data[i]%62];
		*out++ = base62_tab[data[i]/62];
		break;
	case 2:
		bitstring = data[i]<<8|data[i+1];
		*out++ = base62_tab[bitstring%62];
		bitstring /= 62;
		*out++ = base62_tab[bitstring%62];
		*out++ = base62_tab[bitstring/62];
		break;
	case 3:
		bitstring = data[i]<<16|data[i+1]<<8|data[i];
		*out++ = base62_tab[bitstring%62];
		bitstring /= 62;
		*out++ = base62_tab[bitstring%62];
		bitstring /= 62;
		*out++ = base62_tab[bitstring%62];
		bitstring /= 62;
		*out++ = base62_tab[bitstring%62];
		*out++ = base62_tab[bitstring/62];
		break;
	case 4:
		bitstring = data[i]<<24|data[i+1]<<16|data[i+2]<<8|data[i];
		*out++ = base62_tab[bitstring%62];
		bitstring /= 62;
		*out++ = base62_tab[bitstring%62];
		bitstring /= 62;
		*out++ = base62_tab[bitstring%62];
		bitstring /= 62;
		*out++ = base62_tab[bitstring%62];
		bitstring /= 62;
		*out++ = base62_tab[bitstring%62];
		*out++ = base62_tab[bitstring/62];
		break;
	}
	return (int)(out-start);
}
