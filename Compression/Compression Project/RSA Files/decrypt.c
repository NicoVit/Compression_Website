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
//creates the main
int main(int argc, char **argv){
	//makes the vars with defaults
	int opt;
	FILE *inf = stdin;
        FILE *outf = stdout;
	char *priv = "rsa.priv";
	int verbose = 0;
	//loops through options
        while((opt = getopt(argc, argv, OPTIONS)) != -1){
		//switch to case
                switch(opt){
			case 'i':
				//opens and sets file name
				inf = fopen(optarg, "r");
				continue;
			case 'o':
				//opens and sets the file name
				outf = fopen(optarg, "w");
				continue;
			case 'n':
				//sets name
				priv = optarg;
				continue;
			case 'v':
				//flags verbose
				verbose += 1;
				continue;
			case 'h':
				//prints the help message
				fprintf(stderr, "Usage: ./decrypt [options]\n");
                                fprintf(stderr, "  ./decrypt decrypts an input file using the specified private key file,\n");
                                fprintf(stderr, "  writing the result to the specified output file.\n");
                                fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
                                fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
                                fprintf(stderr, "    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n");
                                fprintf(stderr, "    -v          : Enable verbose output.\n");
                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 0;
			default :
				//prints help are returns error
				fprintf(stderr, "Usage: ./decrypt [options]\n");
                                fprintf(stderr, "  ./decrypt decrypts an input file using the specified private key file,\n");
                                fprintf(stderr, "  writing the result to the specified output file.\n");
                                fprintf(stderr, "    -i <infile> : Read input from <infile>. Default: standard input.\n");
                                fprintf(stderr, "    -o <outfile>: Write output to <outfile>. Default: standard output.\n");
                                fprintf(stderr, "    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n");
                                fprintf(stderr, "    -v          : Enable verbose output.\n");
                                fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
				return 1;
		}
	}
	//sets up file and opens it
	FILE *private;
	private = fopen(priv, "r");
	//checks if the file is real
	if(private == NULL){
		//returns of wrong
		fprintf(stderr,"file null error");
		return 1;
	}
	//sets up the relevant vars
	mpz_t n;
	mpz_init(n);
	mpz_t d;
	mpz_init(d);
	//sends everything to be read 
	rsa_read_priv(n,d,private);
	//checks for verbose flag
        if(verbose >= 1){
		//sets bits for the verbose
        	uint64_t bits1 = mpz_sizeinbase(n,2);
        	uint64_t bits2 = mpz_sizeinbase(d,2);
		//prints the verbose
                gmp_fprintf(stderr,"n (%"PRIu64" bits): %Zu\n",bits1, n);
                gmp_fprintf(stderr,"d (%"PRIu64" bits): %Zu\n",bits2, d);
        }
	//decrypts the info
	rsa_decrypt_file(inf,outf,n,d);
	//closes and clears everything used
	fclose(private);
	fclose(inf);
	fclose(outf);
	mpz_clears(n,d,NULL);
}
