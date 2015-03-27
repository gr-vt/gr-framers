/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_FRAMERS_GR_HDLC_DEFRAMER_B_H
#define INCLUDED_FRAMERS_GR_HDLC_DEFRAMER_B_H

#include <framers/api.h>
#include <gnuradio/block.h>
#include <gnuradio/blocks/pdu.h>

namespace gr {
  namespace framers {

    /*!
     * \brief <+description of block+>
     * \ingroup framers
     *
     */
    class FRAMERS_API gr_hdlc_deframer_b : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<gr_hdlc_deframer_b> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of framers::gr_hdlc_deframer_b.
       *
       * To avoid accidental use of raw pointers, framers::gr_hdlc_deframer_b's
       * constructor is in a private implementation
       * class. framers::gr_hdlc_deframer_b::make is the public interface for
       * creating new instances.
       */
      static sptr make(int dlci);
    };

  } // namespace framers
} // namespace gr

#endif /* INCLUDED_FRAMERS_GR_HDLC_DEFRAMER_B_H */

