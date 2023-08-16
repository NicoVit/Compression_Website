#include <stdio.h>
#include <inttypes.h>
#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
//creates the options
#define OPTIONS "i:o:n:vh"
//creates the main func
int main(int argc, char **argv){
	//sets up vars with defaults
	int opt;
	FILE *in = stdin;
        FILE *out = stdout;
	char *pub = "rsa.pub";
	int verbose = 0;
	//goes through the options
        while((opt = getopt(argc, argv, OPTIONS)) != -1){
		//sends to proper case
                switch(opt){
			case 'i':
				//takes file name and opens 
				in = fopen(optarg, "r");
				continue;
			case 'o':
				//takes file name and opens
				out = fopen(optarg, "w");
				continue;
			case 'n':
				//takes name
				pub = optarg;
				continue;
			case 'v':
				//flags verbose
				verbose += 1;
				continue;
			case 'h':
				//prints help message
				fprintf(stderr, "Usage: ./encrypt [options]\n");
                                fprintf(stderr, "  ./encrypt encrypts an input file using the specified public key file,\n");
                                fprintf(stderr, "  writing the result to the specified output file.\n");
                                fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
                                fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
                                fprintf(stderr, "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n");
                                fprintf(stderr, "    -v          : Enable verbose output.\n");
                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 0;
			default :
				//prints default help and returns with error
				fprintf(stderr, "Usage: ./encrypt [options]\n");
                                fprintf(stderr, "  ./encrypt encrypts an input file using the specified public key file,\n");
                                fprintf(stderr, "  writing the result to the specified output file.\n");
                                fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
                                fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
                                fprintf(stderr, "    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n");
                                fprintf(stderr, "    -v          : Enable verbose output.\n");
                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 1;
		}
	}
	//sets up file and opens it
	FILE *public;
	public = fopen(pub, "r");
	//checks if the file is real
	if(public == NULL){
		//returns error and prints message
		fprintf(stderr,"file null error");
		return 1;
	}
	//sets up relevant vars
	mpz_t n;
	mpz_init(n);
	mpz_t e;
	mpz_init(e);
	mpz_t s;
	mpz_init(s);
	char *user = getenv("USER");
	//sends everything to read pub to be processed
	rsa_read_pub(n,e,s,user,public);
	//checks if verbose is flagged
        if(verbose >= 1){
		//sets the bits up
		uint64_t bits1 = mpz_sizeinbase(s,2);
        	uint64_t bits2 = mpz_sizeinbase(n,2);
        	uint64_t bits3 = mpz_sizeinbase(e,2);
		//prints if triggered
                gmp_fprintf(stderr,"username:  %s\n", user);
                gmp_fprintf(stderr,"user signature (%"PRIu64" bits): %Zu\n",bits1,s);
                gmp_fprintf(stderr,"n (%"PRIu64" bits): %Zu\n",bits2, n);
                gmp_fprintf(stderr,"e (%"PRIu64" bits): %Zu\n",bits3, e);
        }
	//makes the rest of the vars
	mpz_t username;
	mpz_init(username);
	//sets to str mpz
	mpz_set_str(username,user,62);
	//verifies the data
	if(rsa_verify(username,s,e,n) == false){
		//returns error if bad
		fprintf(stderr,"could not verify");
		return 1;
	}
	//encrypts the info
	rsa_encrypt_file(in,out,n,e);
	//closes and clears everything used
	fclose(public);
	fclose(in);
	fclose(out);
	mpz_clears(username,n,e,s,NULL);
}
