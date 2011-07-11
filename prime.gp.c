/******************************************************************************/
/**                     Prime-related arithmetic functions                   **/
/******************************************************************************/

// TODO: Optimize multi-word inputs
long
issemiprime(GEN n)
{
	if (typ(n) != t_INT)
		pari_err(arither1, "issemiprime");
	if (signe(n) <= 0)
		return 0;
	
	ulong nn = itou_or_0(n);
	if (nn)
		return uissemiprime(nn);
		
	pari_sp ltop = avma;
	if (!mpodd(n)) {
		long ret = mod4(n) && isprime(shifti(n, -1));
		avma = ltop;
		return ret;
	}
	
	long p = 0;
	byteptr primepointer = diffptr;
	NEXT_PRIME_VIADIFF(p, primepointer);	// Skip 2
	for (;;)
	{
		NEXT_PRIME_VIADIFF(p, primepointer);
		if (p > 997)	// What is a good breakpoint here?
			break;
		if (dvdis(n, p))
		{
			long ret = isprime(diviuexact(n, p));
			avma = ltop;
			return ret;
		}
	}
	
	if (isprime(n))
		return 0;
	
	if (DEBUGLEVEL > 3)
		pari_printf("issemi: Number is a composite with no small prime factors; using general factoring mechanisms.");
	
	GEN fac = Z_factor_until(n, shifti(n, -1));	// Find a nontrivial factor -- returns just the factored part
	GEN expo = gel(fac, 2);
	GEN pr = gel(fac, 1);
	long len = glength(expo);
	if (len > 2) {
		avma = ltop;
		return 0;
	}
	if (len == 2) {
		if (cmpis(gel(expo, 1), 1) > 0 || cmpis(gel(expo, 2), 1) > 0) {
			avma = ltop;
			return 0;
		}
		GEN p = gel(pr, 1);
		GEN q = gel(pr, 2);
		long ret = equalii(mulii(p, q), n) && isprime(p) && isprime(q);
		avma = ltop;
		return ret;
	}
	if (len == 1) {
		long e = itos(gel(expo, 1));
		if (e == 2) {
			GEN p = gel(pr, 1);
			long ret = equalii(sqri(p), n) && isprime(p);
			avma = ltop;
			return ret;
		} else if (e > 2) {
			avma = ltop;
			return 0;
		}
		GEN p = gel(pr, 1);
		long ret = isprime(p) && isprime(diviiexact(n, p));
		avma = ltop;
		return ret;
	}
	
	pari_err(talker, "Z_factor_until returned an unexpected value %Ps at n = %Ps, exiting...", fac, n);
	avma = ltop;
	return NEVER_USED;
}


void
dostuff(GEN lm) {
	byteptr d = diffptr, d1 = diffptr;
	ulong p = 0, q = 0;
	ulong lim = maxprime();
	if (lim < 1000000)
		pari_err(primer1, "<-- you really should have a billion primes precalculated for this function, but you need at least a million or so.");
	lim -= 1000000;
	
	lim = minuu(itou_or_0(lm), lim);

	ulong sum = 0;	// Is a ulong big enough?  Needs to hold approximately p^2 log p.
	for(;;)
	{
		ulong pdiff = *d + 1;	// Increase number of primes by g_n: lose one at the beginning and add g_n + 1 at the end
		NEXT_PRIME_VIADIFF(p,d);
		while(pdiff) {
			NEXT_PRIME_VIADIFF(q,d1);
			sum += q;
			pdiff--;
		}
		sum -= p;
		
		if (uisprime(sum))
			printf("%llu, ", (long long unsigned int)p);

		if (q > lim) break;
	}
	
	pari_printf("\n");
}

// FIXME: Handle the case of small primelimit
/*
P=primorial(661)/2;
v=vector(10^4);i=0;forstep(n=2^64-1,1,-2,if(1==gcd(n,P),v[i++]=n;if(i==#v,return(#v))))
#
sum(i=1,#v,issemi(v[i]))
v=vector(10^5);i=0;forstep(n=2^64-1,1,-2,if(1==gcd(n,P),v[i++]=n;if(i==#v,return(#v))))
sum(i=1,#v,issemi(v[i]))

sum(i=1,#v,print(v[i]);issemi(v[i]))

No rho, crossover  661: 27.68s for 10k, 4:38.21s for 100k
   Rho, crossover  661: 28.06s for 10k, 4:45.50s for 100k
   Rho, crossover 1000: 28.05s for 10k, 4:43.82s for 100k
   Rho, crossover 3000: 27.98s for 10k, 4:44.21s for 100k

With prime test fronting:
   Rho, crossover  661: 13.60s for 10k, 2:12.36s for 100k
   Rho, crossover 1000: 13.72s for 10k, 2:12.53s for 100k
   Rho, crossover 1500: 13.36s for 10k, 2:12.98s for 100k
   Rho, crossover 2000: 13.65s for 10k, 2:12.80s for 100k
   Rho, crossover 2500: 13.57s for 10k, 2:12.98s for 100k
   Rho, crossover 3000: 13.50s for 10k, 2:13.30s for 100k
   Rho, crossover 4000: 13.48s for 10k, 2:12.98s for 100k
   Rho, crossover 9000: 13.54s for 10k, 2:14.17s for 100k

With prime test fronting, before even rho:
   Rho, crossover  661: 12.43s for 10k, 2:01.02s for 100k
   Rho, crossover  750: 12.50s for 10k, 2:01.03s for 100k
   Rho, crossover 1000: 12.38s for 10k, 2:00.59s for 100k
   Rho, crossover 1250: 12.40s for 10k, 2:01.12s for 100k
   Rho, crossover 1500: 12.38s for 10k, 2:00.77s for 100k
   Rho, crossover 2000: 12.58s for 10k, 2:01.75s for 100k
   Rho, crossover 3000: 12.42s for 10k, 2:01.08s for 100k
*/
long
uissemiprime(ulong n)
{
#define CUTOFF 1000UL
#ifdef LONG_IS_64BIT
	#if CUTOFF <= 2642245UL
		#define CUTOFF_CUBE CUTOFF * CUTOFF * CUTOFF
	#else
		#define CUTOFF_CUBE 18446744073709551615UL
	#endif
#else
	#if CUTOFF <= 1625UL
		#define CUTOFF_CUBE CUTOFF * CUTOFF * CUTOFF
	#else
		#define CUTOFF_CUBE 4294967295UL
	#endif
#endif
#if CUTOFF < 661
	#error uissemiprime misconfigured, needs more primes to use uisprime_nosmalldiv.
#endif

	// Remove even numbers. Half of random inputs are caught here.
	if (!(n&1))
		return uisprime(n >> 1);

	// If n is small, simply test up to the cube root; no need for fancy stuff
	ulong lim;
	if (n <= CUTOFF_CUBE) {
		if (n < 9)
			return 0;
		lim = cuberoot(n);

		long p = 0;
		byteptr primepointer = diffptr;
		NEXT_PRIME_VIADIFF(p, primepointer);	// Skip 2
		for (;;)
		{
			NEXT_PRIME_VIADIFF(p, primepointer);
			if (p > lim)
				break;
			if (n%p == 0)
				return uisprime(n / p);
		}
		
		return !uisprime(n);
	}
	
	// First trial division loop, catches 'easy' numbers.
	// 83% of 'random' odd numbers trapped by this loop for CUTOFF = 661,
	// or more for larger values.
	long p = 0;
	byteptr primepointer = diffptr;
	NEXT_PRIME_VIADIFF(p, primepointer);	// Skip 2
	for (;;)
	{
		NEXT_PRIME_VIADIFF(p, primepointer);
		if (p > CUTOFF)
			break;
		if (n%p == 0)
			return uisprime(n / p);
	}

	// Test for primality. About 27% of 661-rough numbers are caught here.
	if (uisprime_nosmalldiv(n))
		return 0;
	
	// Check for a small prime factor with rho. Catches about 70% of remaining
	// composites, based on testing with CUTOFF = 1000.
	pari_sp ltop = avma;
	GEN fac = pollardbrent(utoipos(n));
	if (fac == NULL) {
		avma = ltop;
	} else if (typ(fac) == t_INT) {
		ulong f = itou(fac);
		avma = ltop;
		return uisprime_nosmalldiv(f) && uisprime_nosmalldiv(n / f);
	} else if (typ(fac) == t_VEC) {
		// TODO: Slight speedup possible by paying attention to format instead
		// of just taking first factor:
		//   "a vector of t_INTs, each triple of successive entries containing
		//   a factor, an exponent (equal to one),  and a factor class (NULL
		//   for unknown or zero for known composite)"
		ulong f = itou(gel(fac, 1));
		avma = ltop;
		return uisprime_nosmalldiv(f) && uisprime_nosmalldiv(n / f);
	}
	
	// Second part of trial division loop: avoids the cube root calculation
	// for numbers with a tiny prime divisor, and allows the use of
	// uisprime_nosmalldiv instead of uisprime.  Neither really matter for
	// hard numbers, but for 'average' numbers the first, at least, is
	// worthwhile.
	lim = cuberoot(n);
	for (;;)
	{
		if (p > lim)
			break;
		if (n%p == 0)
			return uisprime_nosmalldiv(n / p);
		NEXT_PRIME_VIADIFF(p, primepointer);
	}
	
	return 1;
#undef CUTOFF_CUBE
#undef CUTOFF
}


GEN
rad(GEN n)
{
	pari_sp ltop = avma;
	GEN ret;
	if (typ(n) != t_INT)
		pari_err(arither1, "rad");
	if (signe(n) < 0)
		n = negi(n);
	ret = vecprod(gel(Z_factor(n), 1));
	ret = gerepileupto(ltop, ret);
	return ret;
}


// TODO: This is slow and I'm not sure why.
long
isprimepower(GEN n)
{
	pari_sp ltop = avma;
	long ret;
	if (typ(n) != t_INT)
		pari_err(arither1, "isprimepower");
	else if (signe(n) < 1)
		return 0;
	else if (!mod2(n))
		return ispow2(n);
		
	// Deal with small odd values
	ulong nn = itou_or_0(n);
	if (nn)
		return uisprimepower(nn);
	
	long p = 0;
	byteptr primepointer = diffptr;
	NEXT_PRIME_VIADIFF(p, primepointer);	// Skip 2
	for (;;)
	{
		NEXT_PRIME_VIADIFF(p, primepointer);
		if (p >= 103)
			break;
		if (!smodis(n, p))
		{
			ret = !cmpii(n, powuu(p, Z_lval(n, p)));
			avma = ltop;
			return ret;
		}
	}

	Z_isanypower(n, &n);	// Expensive test!
	ret = isprime(n);
	avma = ltop;
	return ret;
}


// n is assumed to be odd and positive.
long
uisprimepower(ulong n)
{
	// CUTOFF should be at least 89 for best performance. Tests suggest that
	// 200-300 is the best range for 64-bit platforms.
#define CUTOFF 199UL
	long ret;
	//if (n < 6)
	//	return n > 0;
	//if (!(n&1))
	//	return hamming_word(n) == 1;

	long p = 0;
	byteptr primepointer = diffptr;
	NEXT_PRIME_VIADIFF(p, primepointer);	// Skip 2
	for (;;)
	{
		NEXT_PRIME_VIADIFF(p, primepointer);
		if (p >= CUTOFF)
			break;
		if (!(n%p))
		{
			pari_sp ltop = avma;
			ret = !cmpui(n, powuu(p, u_lval(n, p)));
			avma = ltop;
			return ret;
		}
	}
	
	if (n < CUTOFF * CUTOFF * CUTOFF)
	{
#if CUTOFF < 102
		if (n < CUTOFF * CUTOFF || uisprime(n))
#else
		if (n < CUTOFF * CUTOFF || u_IsLucasPsP(n))
#endif
			return 1;
		return uissquare(n);
	}

// These are the primes preceeding the appropriate root of ULONG_MAX.
#ifdef LONG_IS_64BIT
	#define ROOT11 53
	#define ROOT9 137
	#define ROOT8 251
	#define ROOT7 563
	#define ROOT5 7129
	#define ROOT4 65521
#else
	#define ROOT11 7
	#define ROOT9 11
	#define ROOT8 13
	#define ROOT7 23
	#define ROOT5 83
	#define ROOT4 251
#endif

	pari_sp ltop = avma;
	// Check for squares, fourth powers, and eighth powers as appropriate.
#if CUTOFF <= ROOT4
	if (uissquareall(n, &n))
#endif
#if CUTOFF <= ROOT8
	if (uissquareall(n, &n))
#endif
	uissquareall(n, &n);
	
	GEN nn = utoi(n);
#if CUTOFF <= ROOT11
	pari_warn(warner, "isprimepower: cutoff marginal, performace suffers");
	Z_isanypower(nn, &nn);
#endif
#if CUTOFF <= ROOT7
	ulong mask = 7;
	if (n < CUTOFF * CUTOFF * CUTOFF * CUTOFF * CUTOFF * CUTOFF * CUTOFF) {
		if (n < CUTOFF * CUTOFF * CUTOFF * CUTOFF * CUTOFF)
			mask = 1;
		else
			mask = 3;
	}
#elif CUTOFF <= ROOT5
	ulong mask = 3;
	if (n < CUTOFF * CUTOFF * CUTOFF * CUTOFF * CUTOFF)
		mask = 1;
#else
	ulong mask = 1;
#endif
#if CUTOFF <= ROOT9
	if (is_357_power(nn, &nn, &mask)) {
		mask = 1;
		is_357_power(nn, &nn, &mask);
	}
#else
	is_357_power(nn, &nn, &mask);
#endif
	
	ret = isprime(nn);
	avma = ltop;
	return ret;
#undef CUTOFF
#undef ROOT11
#undef ROOT9
#undef ROOT8
#undef ROOT7
#undef ROOT5
#undef ROOT4
}


// 2-adic valuation of n
INLINE long
valu(ulong n)
{
#if 1
	return n ? __builtin_ctzl(n) : -1;
#else
	if (n == 0)
		return -1;
	long count = 0;
	while (!(n & 1)) {
		n >>= 1;
		count++;
	}
	return count;
#endif
}


long
isPowerful_small(ulong n)
{
	long u = valu(n);
	if (u == 1)
		return 0;
	n >>= u;
	//if (n < 3)
	//	return 1;
	
	long p = 0, lim = usqrtsafe(usqrtsafe(n));
	byteptr primepointer = diffptr;
	NEXT_PRIME_VIADIFF(p, primepointer);
	for (;p <= lim;)
	{
		NEXT_PRIME_VIADIFF(p, primepointer);
		if (n % p == 0)
		{
			n /= p;
			if (n % p)
				return 0;
			do {
				n /= p;
			} while (n % p == 0);
			lim = usqrtsafe(usqrtsafe(n));
		}
	}
	
	if (n == 1)
		return 1;
	
	// if the input was a powerful number, n the square of a prime or the cube of a prime.
	pari_sp btop = avma;
	long ret = n == 1 || Z_isanypower(utoipos(n), NULL);	// FIXME: Check for correctness and speed, something was wrong here.
	avma = btop;
	return ret;
}


long
isPowerful(GEN n)
{
	if (typ(n) != t_INT)
		pari_err(arither1, "isPowerful");
	ulong nn = itou_or_0(n);
	if (nn)
		return isPowerful_small(nn);

	if (!signe(n) || is_pm1(n))
		return 1;
	if (mod4(n) == 2)
		return 0;
	if (!smodis(n, 3) && smodis(n, 9))
		return 0;
	if (!smodis(n, 5) && smodis(n, 25))
		return 0;
	if (!smodis(n, 7) && smodis(n, 49))
		return 0;
	if (!smodis(n, 11) && smodis(n, 121))
		return 0;
	if (!smodis(n, 13) && smodis(n, 169))
		return 0;
	if (!smodis(n, 17) && smodis(n, 289))
		return 0;

	pari_sp ltop = avma;
	n = shifti(n, -vali(n));
	GEN fac, expo, pr;
	
	while (!is_pm1(n) && !isprime(n)) {
		fac = Z_factor_until(n, shifti(n, -1));	// Find a nontrivial factor -- returns just the factored part
		expo = gel(fac, 2);
		pr = gel(fac, 1);
		long len = glength(expo);
		int i;
		
		for (i = 1; i <= len; i++) {
			GEN q = gel(pr, i);	// The found factor, probably prime
			long e = itos(gel(expo, i));
			GEN nn = n = diviiexact(n, powis(q, e));
			GEN g = gcdii(nn, q);
			if (is_pm1(g)) {
				if (e > 1 || isPowerful(q)) {
					n = nn;
				} else {
					avma = ltop;
					return 0;
				}
			} else {
				// This really shouldn't happen!
				GEN nnn;
				long ee = Z_pvalrem(n, q, &nnn);
				if (ee > 1 || isPowerful(nnn)) {
					n = nnn;
				} else {
					avma = top;
					return 0;
				}
			}
		}
	}
	
	avma = ltop;	// slight abuse of ltop: n is used below
	return is_pm1(n);
}


///////////////////////////////
long
isPowerfulCorrect(GEN n)
{
	if (typ(n) != t_INT)
		pari_err(arither1, "isPowerful");

	if (!signe(n) || is_pm1(n))
		return 1;
	if (mod4(n) == 2)
		return 0;
	if (!smodis(n, 3) && smodis(n, 9))
		return 0;
	if (!smodis(n, 5) && smodis(n, 25))
		return 0;
	if (!smodis(n, 7) && smodis(n, 49))
		return 0;
	if (!smodis(n, 11) && smodis(n, 121))
		return 0;
	if (!smodis(n, 13) && smodis(n, 169))
		return 0;
	if (!smodis(n, 17) && smodis(n, 289))
		return 0;

	pari_sp ltop = avma;
	n = shifti(n, -vali(n));
	GEN fac, expo, pr;
	
	while (!is_pm1(n) && !isprime(n)) {
		fac = Z_factor_until(n, shifti(n, -1));	// Find a nontrivial factor -- returns just the factored part
		expo = gel(fac, 2);
		pr = gel(fac, 1);
		long len = glength(expo);
		int i;
		
		for (i = 1; i <= len; i++) {
			GEN q = gel(pr, i);	// The found factor, probably prime
			long e = itos(gel(expo, i));
			GEN nn = n = diviiexact(n, powis(q, e));
			GEN g = gcdii(nn, q);
			if (is_pm1(g)) {
				if (e > 1 || isPowerful(q)) {
					n = nn;
				} else {
					avma = ltop;
					return 0;
				}
			} else {
				// This really shouldn't happen!
				GEN nnn;
				long ee = Z_pvalrem(n, q, &nnn);
				if (ee > 1 || isPowerful(nnn)) {
					n = nnn;
				} else {
					avma = top;
					return 0;
				}
			}
		}
	}
	
	avma = ltop;	// slight abuse of ltop: n is used below
	return is_pm1(n);
}
///////////////////////////////


long
prp(GEN n, GEN b)
{
	pari_sp ltop = avma;
	if (typ(n) != t_INT)
		pari_err(arither1, "prp");
	if (!b)
		b = gen_2;
	else if (typ(b) != t_INT)
		pari_err(arither1, "prp");
	long ret = gequal1(powgi(gmodulo(b, n), subis(n, 1)));
	avma = ltop;
	return ret;
}


long
sprp(GEN n, GEN b)
{
	pari_sp ltop = avma;
	GEN d;
	long l2;
	if (typ(n) != t_INT)
		pari_err(arither1, "sprp");
	else if (cmpis(n, 3) < 0)
		return cmpis(n, 1) > 0;		// Doesn't like even primes
	if (!b)
		b = gen_2;
	else if (typ(b) != t_INT)
		pari_err(arither1, "sprp");

	d = shifti(n, -1);	// At least 1
	long s = vali(d);
	d = shifti(d, -s);
	s++;
	d = gpow(gmodulo(b, n), d, FAKE_PREC);
	if (gequal1(d))
	{
		avma = ltop;
		return 1;
	}

	pari_sp btop = avma, st_lim = stack_lim(btop, 1);
	long i;
	for (i = 1; i <= s; i++)
	{
		if (gequalm1(d))
		{
			avma = ltop;
			return 1;
		}
		d = gsqr(d);
		if (low_stack(st_lim, stack_lim(btop, 1)))
			gerepileall(btop, 1, &d);
	}
	l2 = gequalm1(d);
	avma = ltop;
	return l2;
}


GEN
sopf(GEN n)
{
	pari_sp ltop = avma;
	GEN f, ret = gen_0;
	long l1;
	if (typ(n) != t_INT)
		pari_err(arither1, "sopf");
	f = Z_factor(n);
	l1 = glength(gel(f, 1));
	long i;
	for (i = 1; i <= l1; ++i)
	{
		ret = addii(ret, gcoeff(f, i, 1));
	}
	ret = gerepileupto(ltop, ret);
	return ret;
}


GEN
sopfr(GEN n)
{
	pari_sp ltop = avma;
	GEN f, ret = gen_0;
	long l1;
	if (typ(n) != t_INT)
		pari_err(arither1, "sopfr");
	f = Z_factor(n);
	l1 = glength(gel(f, 1));
	long i;
	for (i = 1; i <= l1; ++i)
	{
		ret = addii(ret, mulii(gcoeff(f, i, 1), gcoeff(f, i, 2)));
	}
	ret = gerepileupto(ltop, ret);
	return ret;
}


GEN
gpf(GEN n)
{
	if (typ(n) != t_INT)
		pari_err(arither1, "gpf");
	if (is_pm1(n))
		return gen_1;
	if (!signe(n))
		return gen_0;
		// My choice of convention: gpf(0) = 0
	
	pari_sp ltop = avma;
	GEN f, ret;
	f = gel(Z_factor(n), 1);
	ret = gel(f, glength(f));
	ret = gerepileupto(ltop, ret);
	return ret;
}


GEN
prodtree(GEN A, long start, long stop)
{
	pari_sp ltop = avma;
	//pari_sp st_lim = stack_lim(ltop, 1);
	long diff = stop - start;
	if (diff >= 8) {
		diff >>= 1;
		GEN leftprod = prodtree(A, start, start + diff);
		//if (low_stack(st_lim, stack_lim(ltop, 1)))
			leftprod = gerepileupto(ltop, leftprod);
		GEN rightprod = prodtree(A, start + diff + 1, stop);
		//if (low_stack(st_lim, stack_lim(ltop, 1)))
			gerepileall(ltop, 2, &leftprod, &rightprod);
		GEN ret = mulii(leftprod, rightprod);
		ret = gerepileupto(ltop, ret);
		return ret;
	}
	
	GEN ret = NEVER_USED, a, b, c, d;
	switch (diff) {
		case 7:
			a = mulss(A[start], A[start+7]);
			b = mulss(A[start+1], A[start+6]);
			c = mulss(A[start+2], A[start+5]);
			d = mulss(A[start+3], A[start+4]);
			ret = mulii(mulii(a, b), mulii(c, d));
			break;
		case 6:
			a = mulss(A[start], A[start+3]);
			b = mulss(A[start+1], A[start+2]);
			c = mulss(A[start+4], A[start+6]);
			ret = mulii(mulii(a, b), mulis(c, A[start+5]));
			break;
		case 5:
			a = mulss(A[start], A[start+5]);
			b = mulss(A[start+1], A[start+4]);
			ret = mulii(mulis(a, A[start+2]), mulis(b, A[start+3]));
			break;
		case 4:
			a = mulss(A[start], A[start+2]);
			b = mulss(A[start+3], A[start+4]);
			ret = mulii(mulis(a, A[start+1]), b);
			break;
		case 3:
			a = mulss(A[start], A[start+3]);
			b = mulss(A[start+1], A[start+2]);
			ret = mulii(a, b);
			break;
		default:
			pari_err(talker, "prodtree passed small argument");
	}
	ret = gerepileupto(ltop, ret);
	return ret;
}


GEN
primorial(GEN n)
{
	static long smallpr[] = {
		1, 1, 2, 6, 6, 30, 30, 210, 210, 210, 210, 2310, 2310, 30030, 30030, 30030,
		30030, 510510, 510510, 9699690, 9699690, 9699690, 9699690, 223092870,
		223092870, 223092870, 223092870, 223092870, 223092870
#ifdef LONG_IS_64BIT
		, 6469693230, 6469693230, 200560490130, 200560490130, 200560490130,
		200560490130, 200560490130, 200560490130
#endif
	};
	
	pari_sp ltop = avma;
	long nn = NEVER_USED;
	GEN ret;
	if (typ(n) == t_REAL) {
		nn = itos_or_0(floorr(n));
		avma = ltop;
	} else if (typ(n) == t_INT) {
		nn = itos_or_0(n);
	} else {
		pari_err(arither1, "primorial");
	}
	
	if (signe(n) <= 0)
		return gen_1;
	if (nn > maxprime() || nn == 0)	// nn == 0 if n didn't fit into a word
		pari_err(primer1, n);
	if (nn < 37) {
#ifdef LONG_IS_64BIT
		ret = stoi(smallpr[nn]);
#else
		avma = ltop;
		if (nn < 29)
			ret = stoi(smallpr[nn]);
		else if (nn < 31)
			ret = uu32toi(1, 2174725934);
		else
			ret = uu32toi(46, 2991994514);
#endif		
		return ret;
	}

	ulong primeCount = uprimepi(nn);
	GEN pr = primes_zv(primeCount);
	ret = prodtree(pr, 1, primeCount);	// Possible TODO: Free memory from latter half of array once its product is calculated?
	ret = gerepileupto(ltop, ret);
	return ret;
}


GEN
lpf(GEN n)
{
	pari_sp ltop = avma;
	GEN res;
	if (typ(n) != t_INT)
		pari_err(arither1, "lpf");
	if (!signe(n))
		return gen_0;	// My choice of convention: lpf(0) = 0
	if (!mod2(n))
		return gen_2;
	if (cmpis(n, 2) < 0) {
		if (cmpis(n, -1) >= 0)
			return gen_1;
		n = negi(n);
	}

	long p = 0;
	byteptr primepointer = diffptr;
	NEXT_PRIME_VIADIFF(p, primepointer);
	for (;p < 9999;)	// TODO: Find appropriate cutoff here
	{
		NEXT_PRIME_VIADIFF(p, primepointer);
		if (dvdis(n, p))
		{
			avma = ltop;
			return stoi(p);
		}
		NEXT_PRIME_VIADIFF(p, primepointer);
		if (dvdis(n, p))
		{
			avma = ltop;
			return stoi(p);
		}
	}
	res = gcoeff(Z_factor(n), 1, 1);	// TODO: Partial factorization?  Tricky to do right...
	res = gerepileupto(ltop, res);
	return res;
}

// TODO: Would be much more efficient to just walk through the primelist...
ulong
ucomposite(long n)
{
	if (n < 2) {
		if (n < 1)
			pari_err(talker, "n-th composite meaningless if n = %ld", n);
		return 4;
	}
	double l = 1.0/log(n);
	
	// Series apparently due to Bojarincev 1967; more terms are known if desired
	ulong c = (ulong)(n * (1 + l * (1 + 2*l * (1 + 2*l))));
	int i = 0;
	int limit = (int)(n - c + uprimepi(c));
	for (; i <= limit; i++)
		if (uisprime(++c))
			i--;
	return c;
}
GEN
composite(long n) { return utoipos(ucomposite(n)); }
