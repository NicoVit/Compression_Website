#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <inttypes.h>
//creates the extrnal vars
extern gmp_randstate_t state;
gmp_randstate_t state;
//creates the init func
void randstate_init(uint64_t seed){
	//loads and creates seed
	srandom(seed);
	gmp_randinit_mt(state);
	gmp_randseed_ui(state, seed);

}
//creates the clear func
void randstate_clear(void){
	//calls the clear
	gmp_randclear(state);
}
