/* Copyright 2016 Libaudioverse Developers. See the COPYRIGHT
file at the top-level directory of this distribution.

Licensed under the mozilla Public License, version 2.0 <LICENSE.MPL2 or
https://www.mozilla.org/en-US/MPL/2.0/> or the Gbnu General Public License, V3 or later
<LICENSE.GPL3 or http://www.gnu.org/licenses/>, at your option. All files in the project
carrying such notice may not be copied, modified, or distributed except according to those terms. */

/**Implement a generic IIR filter.*/
#include <algorithm>
#include <libaudioverse/implementations/iir.hpp>
#include <libaudioverse/private/macros.hpp>
#include <libaudioverse/implementations/biquad.hpp> //for biquadConfigurationImplementation
//for biquad types.
#include <libaudioverse/libaudioverse_properties.h>
//for error codes
#include <libaudioverse/libaudioverse.h>
#include <math.h>
#include <libaudioverse/private/constants.hpp>
#include <stdio.h>
#include <string.h>
#include <libaudioverse/private/error.hpp>

namespace libaudioverse_implementation {

IIRFilter::IIRFilter(double sr) {
	this->sr = sr;
}

void IIRFilter::configure(int newNumeratorLength, double* newNumerator, int newDenominatorLength, double* newDenominator) {
	if(newNumeratorLength == 0 || newDenominatorLength == 0) ERROR(Lav_ERROR_RANGE, "Both numerator and denominator must have nonzero length.");
	//we normalize by the first coefficient but throw it out; consequently, it must be nonzero.
	if(newDenominator[0] == 0.0) ERROR(Lav_ERROR_RANGE);
	if(numerator_length != newNumeratorLength || denominator_length != newDenominatorLength) { //only clear these if absolutely necessary.
		if(history) delete[] history;
		if(numerator) delete[] numerator;
		if(denominator) delete[] denominator;
		if(recursion_history) delete[] recursion_history;
		history = new double[newNumeratorLength]();
		recursion_history = new double[newDenominatorLength]();
		numerator = new double[newNumeratorLength]();
		denominator = new double[newDenominatorLength]();
	}
	std::copy(newNumerator, newNumerator+newNumeratorLength, numerator);
	std::copy(newDenominator, newDenominator+newDenominatorLength, denominator);
	numerator_length= newNumeratorLength;
	denominator_length = newDenominatorLength;
	for(int i = 0; i <denominator_length; i++) denominator[i]/=denominator[0];
}

void IIRFilter::clearHistories() {
	if(numerator_length) memset(history, 0, sizeof(double)*numerator_length);
	if(denominator_length) memset(recursion_history, 0, sizeof(double)*denominator_length);
}

void IIRFilter::setGain(double gain) {
	this->gain = gain;
}

float IIRFilter::tick(float sample) {
	int i;
	history[0] = sample*gain;
	recursion_history[0] = 0.0;
	for(i= numerator_length-1; i > 0; i--) {
		recursion_history[0]+=history[i]*numerator[i];
		history[i]=history[i-1];
	}
	recursion_history[0]+=history[0]*numerator[0];
	for(i = denominator_length-1 ; i >0; i--) {
		recursion_history[0]+=-denominator[i]*recursion_history[i];
		recursion_history[i]=recursion_history[i-1];
	}
	return (float)recursion_history[0];
}

void IIRFilter::configureBiquad(int type, double frequency, double dbGain, double q) {
	double a0, a1, a2, b0, b1, b2;
	biquadConfigurationImplementation(sr, type, frequency, dbGain, q, b0, b1, b2, a0, a1, a2);
	double numerator[] = {1, b1/b0, b2/b0};
	double denominator[] = {1, a1/a0, a2/a0};
	configure(3, numerator, 3, denominator);
	setGain(b0/a0);
}

double IIRFilter::qFromBw(double frequency, double bw) {
	return frequency/bw;
}

double IIRFilter::qFromS(double dbgain, double s) {
	double a=pow(10.0, dbgain/40.0);
	return sqrt((a+1/a)*(1/s-1) + 2);
}

}