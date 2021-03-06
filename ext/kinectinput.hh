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

#include <map>
#include "error.hh"
#include "frame.hh"
#include "kinectcontext.hh"

class KinectInput
{
public:
  KinectInput( KinectContextPtr context, int node ) throw (Error);
  virtual ~KinectInput(void);
  void close(void);
  FramePtr readVideo(void) throw (Error);
  FramePtr readDepth(void) throw (Error);
  bool status(void) const;
  std::string inspect(void) const;
  void setLED( unsigned char state ) throw (Error);
  void setTilt( double angle ) throw (Error);
  void getState(void) throw (Error);
  double getAcc( int id ) throw (Error);
  int getTiltStatus(void) throw (Error);
  static VALUE cRubyClass;
  static VALUE registerRubyClass( VALUE module );
  static void deleteRubyObject( void *ptr );
  static VALUE wrapNew( VALUE rbClass, VALUE rbContext, VALUE rbNode );
  static VALUE wrapClose( VALUE rbSelf );
  static VALUE wrapReadVideo( VALUE rbSelf );
  static VALUE wrapReadDepth( VALUE rbSelf );
  static VALUE wrapStatus( VALUE rbSelf );
  static VALUE wrapInspect( VALUE rbSelf );
  static VALUE wrapSetLED( VALUE rbSelf, VALUE rbState );
  static VALUE wrapSetTilt( VALUE rbSelf, VALUE rbAngle );
  static VALUE wrapGetState( VALUE rbSelf );
  static VALUE wrapGetAcc( VALUE rbSelf );
  static VALUE wrapGetTiltStatus( VALUE rbSelf );
 protected:
  void depthCallBack( void *depth, unsigned int timestamp );
  void videoCallBack( void *video, unsigned int timestamp );
  static void staticDepthCallBack( freenect_device *device,
                                   void *depth, uint32_t timestamp );
  static void staticVideoCallBack( freenect_device *device,
                                   void *rgb, uint32_t timestamp );
  static std::map< freenect_device *, KinectInput * > instances;
  KinectContextPtr m_context;
  int m_node;
  freenect_device *m_device;
  int m_currentRGB;
  bool m_haveRGB;
  char *m_rgb[3];
  int m_currentDepth;
  bool m_haveDepth;
  char *m_depth[3];
};

typedef boost::shared_ptr< KinectInput > KinectInputPtr;

#endif

