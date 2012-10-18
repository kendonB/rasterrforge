/* Robert Hijmans, October 2011 */

#include <R.h>
#include <Rinternals.h>
#include <stdio.h>
#include <stdlib.h>
#include "Rdefines.h"
#include "R_ext/Rdynload.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a,b) ((a)>(b)?(a):(b))


SEXP aggregate(SEXP d, SEXP op, SEXP na, SEXP dim) {
					
	R_len_t i;
	SEXP v;
	double *xd, *xv;
	int *cnts, row, col, newrow, newcol, newcell, mean, expected;

	int oper = INTEGER(op)[0];
	int narm = INTEGER(na)[0];
	
//	int nrin = INTEGER(dim)[0];
	int ncin = INTEGER(dim)[1];
	int nrout = INTEGER(dim)[2];
	int ncout = INTEGER(dim)[3];
	int xfact = INTEGER(dim)[4];
	int yfact = INTEGER(dim)[5];
	
	PROTECT(d = coerceVector(d, REALSXP));
	xd = REAL(d);
	
	if (oper==1) {
		oper = 0;
		mean = 1;
	} else {
		mean = 0;
	}
	
	cnts=(int *) malloc(nrout*ncout*sizeof(int));
	PROTECT( v = allocVector(REALSXP, nrout*ncout) );
	xv = REAL(v);
	
	if (oper==2) {
		for (i=0; i<length(v); i++) {
			xv[i] = R_PosInf;
			cnts[i] = 0;
		}
	
	} else if (oper ==3) {
		for (i=0; i<length(v); i++) {
			xv[i] = R_NegInf;
			cnts[i] = 0;
		}
	
	} else {
		for (i=0; i<length(v); i++) {
			xv[i] = 0;
			cnts[i] = 0;
		}
	}
	
	
	for (i=0; i<length(d); i++) {
		if (R_FINITE(xd[i])) {
			row = i / ncin;
			col = i - (row*ncin);
			newrow = row / yfact;
			newcol = col / xfact;
			// if expand=FALSE
			if ((newcol < ncout) & (newrow < nrout)) {
				newcell = newrow * ncout + newcol;
				if (oper==0) { // sum or mean
					xv[newcell] = xv[newcell] + xd[i];
				} else if (oper==2) {
					xv[newcell] = min(xv[newcell], xd[i]);
				} else if (oper==3) {
					xv[newcell] = max(xv[newcell], xd[i]);
				}
				cnts[newcell] = cnts[newcell] + 1;
			}
		}
	}
	//Rprintf ("newcell = %i \n", newcell);
	if (mean==1) {
		if (narm==0) {
			expected = xfact * yfact;
			for (i=0; i<length(v); i++) {
				if (cnts[i] < expected) {
					xv[i] = R_NaReal;
				} else {
					xv[i] = xv[i] / cnts[i];
				}
			}
		} else {
			for (i=0; i<length(v); i++) {
				if (cnts[i] == 0) {
					xv[i] = R_NaReal;
				} else {
					xv[i] = xv[i] / cnts[i];
				}
			}
		}
	} else if (narm==0) {
		expected = xfact * yfact;
		for (i=0; i<length(v); i++) {
			if (cnts[i] < expected) {
				xv[i] = R_NaReal;
			}
		}
	} else {
		for (i=0; i<length(v); i++) {
			if (cnts[i] == 0) {
				xv[i] = R_NaReal;
			}
		}
	}	
	UNPROTECT(2);
	return(v);
}