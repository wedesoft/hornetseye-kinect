/* HornetsEye - Computer Vision with Ruby
   Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011   Jan Wedekind

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */
#ifndef HORNETSEYE_KINECTINPUT_HH
#define HORNETSEYE_KINECTINPUT_HH

#include "error.hh"
#include "frame.hh"
#include "kinectcontext.hh"

class KinectInput
{
public:
  KinectInput( KinectContextPtr context, int node ) throw (Error);
  virtual ~KinectInput(void);
  void close(void);
  bool status(void) const;
  std::string inspect(void) const;
  static VALUE cRubyClass;
  static VALUE registerRubyClass( VALUE module );
  static void deleteRubyObject( void *ptr );
  static VALUE wrapNew( VALUE rbClass, VALUE rbDevice );
  static VALUE wrapClose( VALUE rbSelf );
  static VALUE wrapStatus( VALUE rbSelf );
 protected:
  int m_node;
  freenect_device *m_device;
};

#endif

