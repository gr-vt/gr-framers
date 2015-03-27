/* -*- c++ -*- */

#define FRAMERS_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "framers_swig_doc.i"

%{
#include "framers/gr_hdlc_framer_b.h"
#include "framers/gr_hdlc_deframer_b.h"
#include "framers/gr_hdlc_framer_2_channel_b.h"
%}


%include "framers/gr_hdlc_framer_b.h"
GR_SWIG_BLOCK_MAGIC2(framers, gr_hdlc_framer_b);
%include "framers/gr_hdlc_deframer_b.h"
GR_SWIG_BLOCK_MAGIC2(framers, gr_hdlc_deframer_b);

%include "framers/gr_hdlc_framer_2_channel_b.h"
GR_SWIG_BLOCK_MAGIC2(framers, gr_hdlc_framer_2_channel_b);
