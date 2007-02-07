/*
  Copyright (c) 2005-2006 Gordon Gremme <gremme@zbh.uni-hamburg.de>
  Copyright (c) 2005-2006 Center for Bioinformatics, University of Hamburg
  See LICENSE file or http://genometools.org/license.html for license details.
*/

#ifndef COIN_HMM

#include "alpha.h"
#include "hmm.h"

typedef enum {
  COIN_FAIR,
  COIN_LOADED,
  COIN_NUM_OF_STATES
} Coin_states;

typedef enum {
  HEAD, /* "Hh" */
  TAIL, /* "Tt" */
  COIN_NUM_OF_SYMBOLS
} Coin_emissions;

HMM*   coin_hmm_loaded(void);
HMM*   coin_hmm_fair(void);
Alpha* coin_hmm_alpha(void);

#endif
