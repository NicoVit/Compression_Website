#include <stdio.h>
#include "numtheory.h"
#include <inttypes.h>
#include <gmp.h>
#include <stdbool.h>
#include "randstate.h"
#include <stdlib.h>
//creates the make pub func
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters){
        //creates the temps and required vars
	uint64_t bits1;
        uint64_t bits2;
	//allocates bits
        bits1 = (random() % (((3*nbits)/4) - (nbits/4) + 1)) + (nbits/4);
	bits2 = nbits - bits1;
	//gets primes
        make_prime(p,bits1,iters);
        make_prime(q,bits2,iters);
        mpz_t l_gcd;
        mpz_init(l_gcd);
        mpz_t r_gcd;
        mpz_init(r_gcd);
	//creates the specific vars
        mpz_sub_ui(l_gcd,p,1);
        mpz_sub_ui(r_gcd,q,1);
	//multis p and q
        mpz_mul(n, p, q);
        mpz_t added;
        mpz_init(added);
	//adds p and q
        mpz_add(added, p, q);
	//subs added bits from n
        mpz_sub(added, n, added);
	//adds one
        mpz_add_ui(added, added, 1);
        mpz_abs(added, added);
        mpz_t product;
        mpz_init(product);
	//finds the gcd
        gcd(product, l_gcd, r_gcd);
        mpz_t lcm;
        mpz_init(lcm);
        mpz_fdiv_q(lcm, added, product);
        mpz_t hold;
        mpz_init(hold);
        mpz_t temp;
        mpz_init(temp);
	//loops while temp and 1 arent the same or while the bits is less than the nbits
        while((mpz_cmp_ui(temp, 1) != 0) || (mpz_sizeinbase(hold, 2) < nbits)){
		//gets are random num
                mpz_urandomb(hold, state, nbits);
		//sends to the gcd function
                gcd(temp, hold, lcm);
        }
	//sets the output
        mpz_set(e,hold);
	//clears the temps
	mpz_clears(l_gcd,r_gcd,added,product,lcm,hold,temp,NULL);

}
//creates the write pub func
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile){
	//writes each input to the file 
	gmp_fprintf(pbfile, "%Zx\n\n", n);
	gmp_fprintf(pbfile, "%Zx\n\n", e);
	gmp_fprintf(pbfile, "%Zx\n\n", s);
	gmp_fprintf(pbfile, "%s\n\n", username);
}
//cretaes the read pub func
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile){
	//reads the file inputs
	gmp_fscanf(pbfile, "%Zx\n\n", n);
        gmp_fscanf(pbfile, "%Zx\n\n", e);
        gmp_fscanf(pbfile, "%Zx\n\n", s);
        gmp_fscanf(pbfile, "%s\n\n", username);	
}

//creates the make priv func
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q){
        //creates the temp functions
	mpz_t n;
	mpz_init(n);
	mpz_t l_gcd;
        mpz_init(l_gcd);
        mpz_t r_gcd;
        mpz_init(r_gcd);
	//sets up the gcd functions
        mpz_sub_ui(l_gcd,p,1);
        mpz_sub_ui(r_gcd,q,1);
        mpz_mul(n, p, q);
        mpz_t added;
        mpz_init(added);
	//adds p and q
        mpz_add(added, p, q);
	//subs the above from n
        mpz_sub(added, n, added);
	//adds 1
        mpz_add_ui(added, added, 1);
	//finds the absolute value
        mpz_abs(added, added);
        mpz_t product;
        mpz_init(product);
	//finds the gcd
        gcd(product, l_gcd, r_gcd);
        mpz_t lcm;
        mpz_init(lcm);
	//divides the gcd from the added
        mpz_fdiv_q(lcm, added, product);
	//sends to my mod inverse
	mod_inverse(d,e,lcm);
	//clears the temps
	mpz_clears(n,l_gcd,r_gcd,added,product,lcm,NULL);
}

//creates the write priv func
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile){
	//prints to file
	gmp_fprintf(pvfile, "%Zx\n\n", n);
        gmp_fprintf(pvfile, "%Zx\n\n", d);
}
//creates the read priv func
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile){
	//scans the file
	gmp_fscanf(pvfile, "%Zx\n\n", n);
        gmp_fscanf(pvfile, "%Zx\n\n", d);
}
//creates the encrypt func
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n){
	//sets the temp
	mpz_t pow_hold;
	mpz_init(pow_hold);
	//sends eveything to pow mod func
	pow_mod(pow_hold,m,e,n);
	//sets output
	mpz_set(c,pow_hold);
	//clears temp
	mpz_clear(pow_hold);

}
//creates the function for encrypt file
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e){
	//creates the temp vars
	mpz_t k;
	mpz_init(k);
	mpz_t m;
	mpz_init(m);
	mpz_t c;
	mpz_init(c);
	uint64_t j;
	//gets the block size
	uint64_t log = mpz_sizeinbase(n,2);
	//subs 1
	log--;
	//divides by 8
	log = log/8;
	//sets k to the setup
	mpz_set_ui(k,log);
	//creates the array
	uint8_t *arr;
	//checks if at the end of file
	if(feof(infile) == 0){
		//allocates the memory for the chunk
		arr = (uint8_t*)calloc(log,sizeof(uint8_t));
		//sets the workaround bit
		arr[0] = 0xFF;
		//sets up j and reads the file
		j = fread(arr+1,sizeof(uint8_t),log-1,infile);
		//imports to set to mpz t
		mpz_import(m, j+1, 1, sizeof(uint8_t),1,0,arr);
		//encrypts the info
		rsa_encrypt(c,m,e,n);
		//prints to the file
		gmp_fprintf(outfile, "%Zx\n\n", c);
		//frees the chunk
		free(arr);
		//reruns the process
		rsa_encrypt_file(infile,outfile,n,e);
	}
	//clears temps
	mpz_clears(k,m,c,NULL);

}
//creates the decrypt function 
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n){
	//sets the temp up
        mpz_t pow_hold;
        mpz_init(pow_hold);
	// sends everything to pow mod
        pow_mod(pow_hold,c,d,n);
	//sets output
        mpz_set(m,pow_hold);
	//clears temps
        mpz_clear(pow_hold);
}
//creates the decrypt file function
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d){
	//creates the temp vars
	mpz_t k;
        mpz_init(k);
        mpz_t m;
        mpz_init(m);
	uint64_t j;
	//gets block size
        uint64_t log = mpz_sizeinbase(n,2);
        log--;
        log = log/8;
	//creates array
        uint8_t *arr;
	//sets vars
	mpz_set_ui(k,log);
	mpz_t c;
	mpz_init(c);
	//checks if the file is ended
	if(feof(infile) == 0){
		//allocates block size
                arr = (uint8_t*)calloc(log,sizeof(uint8_t));
		//scans the file into mpz
                gmp_fscanf(infile,"%Zx\n\n",c);
		//sends everything to be decrypted
		rsa_decrypt(m,c,d,n);
		//exports everything back into bytes
		mpz_export(arr,&j,1,sizeof(uint8_t),1,0,m);
		//writes the bytes
		fwrite(arr+1,sizeof(uint8_t),j-1,outfile);
		//frees chunk
		free(arr);
		//recursses
		rsa_decrypt_file(infile,outfile,n,d);
        }
	//clears the temps
        mpz_clears(k,m,c,NULL);
}
//creates the signing func
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n){
	//sets the temps up 
	mpz_t pow_hold;
        mpz_init(pow_hold);
	//sends everything to my pow mod
        pow_mod(pow_hold,m,d,n);
	//sets output
        mpz_set(s,pow_hold);
	//clears temps
        mpz_clear(pow_hold);
}
//sets up veriofy func
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n){
	//sets up the temp vars
	bool out = false;
	mpz_t pow_hold;
        mpz_init(pow_hold);
	//sends everything to powmod
        pow_mod(pow_hold,s,e,n);
	//checks the return
	if(mpz_cmp(m,pow_hold) == 0){
		//sets depending on the return
		out = true;
	}
	//clears
        mpz_clear(pow_hold);
	//returns the outcome
	return out;
}
