#pragma once

// Constants
//=======================================================
const unsigned total_canaries = 2;
const char canary_value = 127;
const unsigned Y = 31; //!This constant is used in default hashing algorithm
const unsigned default_parameters_quantity = sizeof(unsigned);
//=======================================================


// Global functions
//=======================================================
unsigned tDefaultHash(char *arg, const unsigned sz);
//=======================================================
