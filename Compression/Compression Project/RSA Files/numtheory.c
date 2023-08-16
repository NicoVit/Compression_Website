#include <stdio.h>
#include <gmp.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include "randstate.h"
//creates the gcd function
void gcd(mpz_t d, mpz_t a, mpz_t b){
	//creates the temp vars
        mpz_t t;
        mpz_init(t);
        mpz_t a_hold;
        mpz_init(a_hold);
        mpz_t b_hold;
        mpz_init(b_hold);
        mpz_set(a_hold,a);
        mpz_set(b_hold,b);
	//loops while the b is not zero
        while(mpz_cmp_si(b_hold,0)!=0){
		//sets temp
                mpz_set(t, b_hold);
		//send the info to the mod function and sets it to b temp
                mpz_mod(b_hold,a_hold,b_hold);
		//updates a hold
                mpz_set(a_hold,t);
        }
	//sets the output
        mpz_set(d,a_hold);
	//clears the temps
        mpz_clears(t,a_hold,b_hold, NULL);
}
//creates the pow mod function
void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus){
	//sets the temp vars
	mpz_t v;
	mpz_t p;
	mpz_init(v);
	mpz_init(p);
	//prepares the specific temps
	mpz_set_si(v, 1);
	mpz_set(p, base);
	mpz_t hold_mul;
	mpz_init(hold_mul);
	mpz_t hold_mul_div;
	mpz_init(hold_mul_div);
	mpz_t hold_exp;
	mpz_init(hold_exp);
	mpz_set(hold_exp,exponent);
	//loops while the exponent is above zero
	while(mpz_cmp_si(hold_exp,0) > 0){
		//checks if the exp is even
		if(mpz_odd_p(hold_exp) != 0){
			//multiplies v and p
			mpz_mul(hold_mul, v, p);
			//sets the mod to v
			mpz_mod(v,hold_mul,modulus);

		}
		//multiplies the p vars
		mpz_mul(hold_mul_div, p, p);
		//mods them 
		mpz_mod(p,hold_mul_div,modulus);
		//divides the outcome from above by w
		mpz_fdiv_q_ui(hold_exp, hold_exp,2);
	}
	//prepares the output
	mpz_set(out, v);
	//clears temp vars
	mpz_clears(v,p,hold_mul,hold_mul_div,hold_exp,NULL);
}
//sets the is prime function up
bool is_prime(mpz_t n, uint64_t iters){
	//checks for edge cases for efficency 
	if(mpz_cmp_si(n,1) == 0 || mpz_cmp_si(n,2) == 0 || mpz_cmp_si(n,3) == 0){
		return true;
	}
	//sets the temp vars
	mpz_t hold;
        mpz_init(hold);
	//prepares the specific ones 
        mpz_sub_ui(hold, n, 3);
	mpz_t s;
	mpz_init(s);
	mpz_t y;
	mpz_init(y);
	mpz_t z;
	mpz_init(z);
	mpz_sub_ui(z, n, 1);
	mpz_t r;
	mpz_init(r);
	mpz_set(r, n);
	mpz_sub_ui(r,r,1);
	mpz_t j;
	mpz_init(j);
	mpz_t a;
	mpz_init(a);
	mpz_t num_one;
        mpz_init(num_one);
	//prepares the nums as mpz types
        mpz_set_ui(num_one, 1);
	mpz_t exp_hold;
	mpz_init(exp_hold);
	mpz_t num_two;
        mpz_init(num_two);
	mpz_set_ui(num_two,2);
	mpz_set_si(exp_hold,1);
	mpz_t div_while;
	mpz_init(div_while);
	mpz_t add_while;
	mpz_init(add_while);
	//loops while r is even
	while(mpz_even_p(r)){
		//divides r by 2
		mpz_fdiv_q(div_while,r,num_two);
		mpz_set(r,div_while);
		//adds s and 1
		mpz_add_ui(add_while,s,1);
		mpz_set(s,add_while);
	}
	//prepares the s minus 1
	mpz_t subtract;
	mpz_init(subtract);
	mpz_sub_ui(subtract, s, 1);
	//loops from 1 through the iters
	for(uint64_t x = 1; x<=iters;x+=1){
		//creates a random num
		mpz_urandomm(a, state, hold);
		//adds 2 
		mpz_add_ui(a,a,2);
		//sends to my pow mod
		pow_mod(y, a,r,n);
		//checks if y and 1 are not equal and y and z not equal	
		if((mpz_cmp_si(y,1) != 0) && (mpz_cmp(y,z)!=0)){
			//sets up j
			mpz_set_si(j,1);
			//loops while j and temp are not the same and y and z
			while(mpz_cmp(j,subtract) <= 0&& mpz_cmp(y,z) !=0){
				//sends the nums to my pow mod
				pow_mod(y,y,num_two,n);
				//checks if outcome is equal to one
				if(mpz_cmp(y,num_one) ==0){
					//returns outcome of false and clears
					mpz_clears(s,y,z,r,j,a,exp_hold,num_two,hold,subtract,num_one,div_while,add_while,NULL);
					return false;
				}
				//adds 1 to j
				mpz_add_ui(j,j,1);
			}
			//checks if y and z are not equal
			if(mpz_cmp(y,z)!=0){
				//returns the false output and clear
				mpz_clears(s,y,z,r,j,a,exp_hold,num_two,hold,subtract,num_one,div_while,add_while,NULL);
				return false;
			}
		}
	}
	//clears the temps and returns true
	mpz_clears(s,y,z,r,j,a,exp_hold,num_two,hold,subtract,num_one,div_while,add_while,NULL);
	return true;
}
//creates the mod inverse function
void mod_inverse(mpz_t i, mpz_t a, mpz_t n){
	//sets up the temp vars
	mpz_t r;
	mpz_init(r);
	mpz_t r_prime;
	mpz_init(r_prime);
	mpz_t t;
	mpz_init(t);
	mpz_t t_prime;
	mpz_init(t_prime);
	mpz_set(r,n);
	mpz_set(r_prime,a);
	//prepares the specific vars
	mpz_set_ui(t, 0);
	mpz_set_ui(t_prime,1);
	mpz_t q;
	mpz_init(q);
	mpz_t hold_div;
	mpz_init(hold_div);
	mpz_t mul;
	mpz_init(mul);
	mpz_t sub;
	mpz_init(sub);
	mpz_t hold_r;
	mpz_t hold_t;
	mpz_init(hold_r);
	mpz_init(hold_t);
	//loops while r prime and 0 are not the same
	while(mpz_cmp_ui(r_prime,0) != 0){
		//divides r and r prime
		mpz_fdiv_q(hold_div,r,r_prime);
		mpz_set(q, hold_div);
		//sets temp
		mpz_set(hold_r, r);
		mpz_set(r, r_prime);
		//multiplies a and r prime
		mpz_mul(mul, q, r_prime);
		//subtracts r and the output above
		mpz_sub(sub, hold_r, mul);
		//sets temps
		mpz_set(r_prime, sub);
		mpz_set(hold_t, t);
		mpz_set(t,t_prime);
		//multiplies q and t prime
		mpz_mul(mul, q, t_prime);
		//subs the ouput from t
                mpz_sub(sub, hold_t, mul);
		//sets output
                mpz_set(t_prime, sub);
	}
	//checks r and 1 for difference 
	if(mpz_cmp_ui(r,1) >0){
		//clears and returns
		mpz_clears(r,r_prime,t,t_prime,q,hold_div,mul,sub,hold_r,hold_t,NULL);
		return;
	}
	//compares t and 0 for differences
	if(mpz_cmp_ui(t,0) <0){
		//adds t and n together
		mpz_add(t, t, n);
	}
	//sets output and clears
	mpz_set(i,t);
	mpz_clears(r,r_prime,t,t_prime,q,hold_div,mul,sub,hold_r,hold_t,NULL);
}
//creates the make prime function
void make_prime(mpz_t p, uint64_t bits, uint64_t iters){
	//sets the temps
	mpz_t num;
	mpz_init(num);
	//creates the random
	mpz_urandomb(num, state, bits);
	//creates odd for speed increase by checking if even and adding one
	if(mpz_even_p(num) != 0){
		mpz_add_ui(num,num,1);
	}
	//checks the bits and prime
	if(mpz_sizeinbase(num,2) >= bits && is_prime(num,iters)){
		//sets the output
		mpz_set(p,num);
		mpz_clear(num);
		return;
	}else{
		//recursses if not prime
		make_prime(p, bits, iters);
	}
	//clears the temp
	mpz_clear(num);
}
