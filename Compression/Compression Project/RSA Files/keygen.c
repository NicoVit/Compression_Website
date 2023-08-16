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
//makes the options
#define OPTIONS "b:i:n:d:s:vh"
//creates the main
int main(int argc, char **argv){
	//creates the temps with the defaults set
	uint64_t bits = 1024;
	uint64_t iters = 50;
	char *pub = "rsa.pub";
	char *priv = "rsa.priv";
	uint64_t seed = time(NULL);
	uint64_t verbose = 0;
	uint64_t bits_hold;
	uint64_t iters_hold;
	int opt;
	//loops through the options
	while((opt = getopt(argc, argv, OPTIONS)) != -1){
		//sends to the proper case
		switch(opt){
			case 'b':
				//sets the hold to check the ranges
				bits_hold = strtol(optarg,NULL,10);
				//checks the ranges
				if(bits_hold >= 50 && bits_hold <= 4096){
					//sets if correct
					bits = strtol(optarg,NULL,10);
					continue;
				}
				//prints help if wrong
				fprintf(stderr,"Usage: ./keygen [options]\n");
                                fprintf(stderr,"  ./keygen generates a public / private key pair, placing the keys into the public and private\n");
                                fprintf(stderr,"  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
                                fprintf(stderr,"  the program options.\n");
                                fprintf(stderr,"    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
                                fprintf(stderr,"    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
                                fprintf(stderr,"    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
                                fprintf(stderr,"    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
                                fprintf(stderr,"    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
                                fprintf(stderr,"    -v          : Enable verbose output.\n");
                                fprintf(stderr,"    -h          : Display program synopsis and usage.\n");
				continue;
			case 'i':
				//sets the temp for the check
				iters_hold = strtol(optarg,NULL,10);
				//checks the ranges
				if(iters_hold >= 1 && iters_hold <= 500){
					//sets the outcome
					iters = strtol(optarg,NULL,10);
					continue;
				}
				//prints for error
				fprintf(stderr,"Usage: ./keygen [options]\n");
                                fprintf(stderr,"  ./keygen generates a public / private key pair, placing the keys into the public and private\n");
                                fprintf(stderr,"  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
                                fprintf(stderr,"  the program options.\n");
                                fprintf(stderr,"    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
                                fprintf(stderr,"    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
                                fprintf(stderr,"    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
                                fprintf(stderr,"    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
                                fprintf(stderr,"    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
                                fprintf(stderr,"    -v          : Enable verbose output.\n");
                                fprintf(stderr,"    -h          : Display program synopsis and usage.\n");
				continue;
			case 'n':
				//sets the name
				pub = optarg;
				continue;
			case 'd':
				//sets the name
				priv = optarg;
				continue;
			case 's':
				//sets the seed
				seed = strtol(optarg,NULL,10);
				continue;
			case 'v':
				//flags verbose
				verbose += 1;
				continue;
			case 'h':
				//prints the help 
				fprintf(stderr,"Usage: ./keygen [options]\n");
				fprintf(stderr,"  ./keygen generates a public / private key pair, placing the keys into the public and private\n");
				fprintf(stderr,"  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
				fprintf(stderr,"  the program options.\n");
				fprintf(stderr,"    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
				fprintf(stderr,"    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
				fprintf(stderr,"    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
				fprintf(stderr,"    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
				fprintf(stderr,"    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
				fprintf(stderr,"    -v          : Enable verbose output.\n");
				fprintf(stderr,"    -h          : Display program synopsis and usage.\n");
				continue;
			default:
				//prints the help
				fprintf(stderr,"Usage: ./keygen [options]\n");
                                fprintf(stderr,"  ./keygen generates a public / private key pair, placing the keys into the public and private\n");
                                fprintf(stderr,"  key files as specified below. The keys have a modulus (n) whose length is specified in\n");
                                fprintf(stderr,"  the program options.\n");
                                fprintf(stderr,"    -s <seed>   : Use <seed> as the random number seed. Default: time()\n");
                                fprintf(stderr,"    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n");
                                fprintf(stderr,"    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n");
                                fprintf(stderr,"    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n");
                                fprintf(stderr,"    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n");
                                fprintf(stderr,"    -v          : Enable verbose output.\n");
                                fprintf(stderr,"    -h          : Display program synopsis and usage.\n");
				//returns if error
				return 1;
		}


	}
	//sets up the files to be used and accessed
	FILE *public;
        public = fopen(pub, "w");
	FILE *private;
        private	= fopen(priv, "w");
	//checks to see if files are real and returns errors if wrong
	if(public == NULL || private == NULL){
		fprintf(stderr,"file null error");
		return 1;
	}
	//checks permissions and returns error message if needed
	if(fchmod(fileno(private),S_IRUSR | S_IWUSR) != 0){
		fprintf(stderr,"file permission error");
		return 1;
	}
	//loads seed
	randstate_init(seed);
	//makes relevant vars
	mpz_t p;
	mpz_init(p);
	mpz_t q;
	mpz_init(q);
	mpz_t n;
	mpz_init(n);
	mpz_t e;
	mpz_init(e);
	mpz_t d;
	mpz_init(d);
	//makes pub key
	rsa_make_pub(p,q,n,e,bits,iters);
	//makes priv key
	rsa_make_priv(d,e,p,q);
	//makes rest of relevant vars
	mpz_t sig;
        mpz_init(sig);
        mpz_t output;
        mpz_init(output);
	char *username = getenv("USER");
	//sets to str base 62
	mpz_set_str(output,username,62);
	//sends everything to sign
	rsa_sign(sig,output,d,n);
	//writes the keys
	rsa_write_pub(n,e,sig,username,public);
	rsa_write_priv(n,d,private);
	//checks if verbose is triggered
	if(verbose >= 1){
		//prepares the bits in verbose
	        uint64_t bits1 = mpz_sizeinbase(sig,2);
        	uint64_t bits2 = mpz_sizeinbase(p,2);
        	uint64_t bits3 = mpz_sizeinbase(q,2);
        	uint64_t bits4 = mpz_sizeinbase(n,2);
        	uint64_t bits5 = mpz_sizeinbase(e,2);
        	uint64_t bits6 = mpz_sizeinbase(d,2);
		//prints in stderr for verbose
		gmp_fprintf(stderr,"username:  %s\n", username);
		gmp_fprintf(stderr,"user signature (%"PRIu64" bits): %Zu\n",bits1,sig);
		gmp_fprintf(stderr,"p (%"PRIu64" bits): %Zu\n",bits2, p);
		gmp_fprintf(stderr,"q (%"PRIu64" bits): %Zu\n",bits3, q);
		gmp_fprintf(stderr,"n (%"PRIu64" bits): %Zu\n",bits4, n);
		gmp_fprintf(stderr,"e (%"PRIu64" bits): %Zu\n",bits5, e);
		gmp_fprintf(stderr,"d (%"PRIu64" bits): %Zu\n",bits6, d);
	}
	//closes and clears everything used
	fclose(public);
	fclose(private);
	randstate_clear();
	mpz_clears(p,q,n,e,d,sig,output,NULL);
	return 0;

}


