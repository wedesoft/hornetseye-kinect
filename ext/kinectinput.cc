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
#include "kinectinput.hh"

#ifndef FREENECT_FRAME_W
#define FREENECT_FRAME_W 640
#endif
#ifndef FREENECT_FRAME_H
#define FREENECT_FRAME_H 480
#endif
#ifndef FREENECT_FRAME_PIX
#define FREENECT_FRAME_PIX (FREENECT_FRAME_W * FREENECT_FRAME_H)
#endif
#ifndef FREENECT_VIDEO_RGB_SIZE
#define FREENECT_VIDEO_RGB_SIZE (FREENECT_FRAME_PIX*3)
#endif
#ifndef FREENECT_DEPTH_11BIT_SIZE
#define FREENECT_DEPTH_11BIT_SIZE (FREENECT_FRAME_PIX*2)
#endif


using namespace std;

VALUE KinectInput::cRubyClass = Qnil;

map< freenect_device *, KinectInput * > KinectInput::instances;

KinectInput::KinectInput( KinectContextPtr context, int node ) throw (Error):
  m_context( context ), m_node( node ), m_device( NULL ), m_currentRGB(0), m_haveRGB(false),
  m_currentDepth(0), m_haveDepth(false)
{
  memset( &m_rgb, 0, sizeof(m_rgb) ); memset( &m_depth, 0, sizeof(m_depth) );
  m_context->lock();
  if ( freenect_open_device( context->get(), &m_device, node ) < 0 ) {
    m_context->unlock();
    ERRORMACRO( false, Error, , "Failed to open Kinect device number " << node );
  };
  for ( int i=0; i<3; i++ ) {
    m_rgb[i] = (char *)malloc( FREENECT_VIDEO_RGB_SIZE );
    m_depth[i] = (char *)malloc( FREENECT_DEPTH_11BIT_SIZE );
  };
  instances[ m_device ] = this;
  // freenect_set_depth_format( m_device, FREENECT_DEPTH_11BIT );
  // freenect_set_video_format( m_device, FREENECT_VIDEO_RGB );
  freenect_set_video_mode(m_device, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB));
  freenect_set_depth_mode(m_device, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT));
  freenect_set_depth_buffer( m_device, m_rgb[0] );
  freenect_set_video_buffer( m_device, m_rgb[0] );
  freenect_set_depth_callback( m_device, staticDepthCallBack );
  freenect_set_video_callback( m_device, staticVideoCallBack );
  freenect_start_depth( m_device );
  freenect_start_video( m_device );
  m_context->addInstance();
  m_context->unlock();
}

KinectInput::~KinectInput(void)
{
  close();
}

void KinectInput::close(void)
{
  if ( m_device != NULL ) {
    m_context->lock();
    freenect_stop_depth( m_device );
    freenect_stop_video( m_device );
    freenect_set_led( m_device, LED_BLINK_GREEN );
    freenect_close_device( m_device );
    instances.erase( m_device );
    m_context->removeInstance();
    m_context->unlock();
    m_context.reset();
    m_device = NULL;
  };
  for ( int i=0; i<3; i++ ) {
    if ( m_depth[i] != NULL ) {
      free( m_depth[i] );
      m_depth[i] = NULL;
    };
    if ( m_rgb[i] != NULL ) {
      free( m_rgb[i] );
      m_rgb[i] = NULL;
    };
  };
  m_node = -1;
}

FramePtr KinectInput::readVideo(void) throw (Error)
{
  ERRORMACRO( m_device != NULL, Error, , "Kinect device is not open. "
              "Did you call \"close\" before?" );
  m_context->lock();
  try {
    while ( !m_haveRGB ) m_context->wait();
    m_haveRGB = false;
    char *data = m_rgb[ 2 ];
    m_rgb[ 2 ] = m_rgb[ 1 - m_currentRGB ];
    m_rgb[ 1 - m_currentRGB ] = data;
  } catch ( Error &e ) {
    m_context->unlock();
    throw e;
  };
  m_context->unlock();
  FramePtr retVal = FramePtr
    ( new Frame( "UBYTERGB", FREENECT_FRAME_W, FREENECT_FRAME_H, m_rgb[ 2 ] ) );
  return retVal;
}

FramePtr KinectInput::readDepth(void) throw (Error)
{
  ERRORMACRO( m_device != NULL, Error, , "Kinect device is not open. "
              "Did you call \"close\" before?" );
  m_context->lock();
  try {
    while ( !m_haveDepth ) m_context->wait();
    m_haveDepth = false;
    char *data = m_depth[ 2 ];
    m_depth[ 2 ] = m_depth[ 1 - m_currentDepth ];
    m_depth[ 1 - m_currentDepth ] = data;
  } catch ( Error &e ) {
    m_context->unlock();
    throw e;
  };
  m_context->unlock();
  FramePtr retVal = FramePtr
    ( new Frame( "USINT", FREENECT_FRAME_W, FREENECT_FRAME_H, m_depth[ 2 ] ) );
  return retVal;
}

bool KinectInput::status(void) const
{
  return m_device != NULL;
}

string KinectInput::inspect(void) const
{
  ostringstream s;
  s << "KinectInput( " << m_node << " )";
  return s.str();
}
 
void KinectInput::setLED( unsigned char state ) throw (Error)
{
  ERRORMACRO( m_device != NULL, Error, , "Kinect device is not open. "
              "Did you call \"close\" before?" );
  m_context->lock();
  if ( freenect_set_led( m_device, (freenect_led_options)state ) == 0 ) {
    m_context->unlock();
  } else {
    m_context->unlock();
    ERRORMACRO( false, Error, , "Error setting LED state" );
  };
}

void KinectInput::setTilt( double angle ) throw (Error)
{
  ERRORMACRO( m_device != NULL, Error, , "Kinect device is not open. "
              "Did you call \"close\" before?" );
  m_context->lock();
  if ( freenect_set_tilt_degs( m_device, angle ) == 0 ) {
    m_context->unlock();
  } else {
    ERRORMACRO( false, Error, , "Error setting tilt angle" );
    m_context->unlock();
  };
}

void KinectInput::getState(void) throw (Error)
{
  ERRORMACRO( m_device != NULL, Error, , "Kinect device is not open. "
              "Did you call \"close\" before?" );
  freenect_update_tilt_state( m_device );
}

double KinectInput::getAcc( int id ) throw (Error)
{
  ERRORMACRO( m_device != NULL, Error, , "Kinect device is not open. "
              "Did you call \"close\" before?" );
  double acc[3];
  freenect_get_mks_accel( freenect_get_tilt_state( m_device ),
                          &acc[0], &acc[1], &acc[2] );
  return acc[ id ];
}

int KinectInput::getTiltStatus(void) throw (Error)
{
  ERRORMACRO( m_device != NULL, Error, , "Kinect device is not open. "
              "Did you call \"close\" before?" );
  return (int)(freenect_get_tilt_state( m_device )->tilt_status);
}

void KinectInput::depthCallBack( void *depth, unsigned int timestamp )
{
  m_currentDepth = 1 - m_currentDepth;
  m_haveDepth = true;
  freenect_set_depth_buffer( m_device, m_depth[ m_currentDepth ] );
}

void KinectInput::videoCallBack( void *video, unsigned int timestamp )
{
  m_currentRGB = 1 - m_currentRGB;
  m_haveRGB = true;
  freenect_set_video_buffer( m_device, m_rgb[ m_currentRGB ] );
}

void KinectInput::staticDepthCallBack( freenect_device *device,
                                       void *depth, uint32_t timestamp )
{
  instances[ device ]->depthCallBack( depth, timestamp );
}

void KinectInput::staticVideoCallBack( freenect_device *device,
                                       void *rgb, uint32_t timestamp )
{
  instances[ device ]->videoCallBack( rgb, timestamp );
}

VALUE KinectInput::registerRubyClass( VALUE module )
{
  cRubyClass = rb_define_class_under( module, "KinectInput", rb_cObject );
  rb_define_const( cRubyClass, "LED_OFF", INT2NUM( LED_OFF ) );
  rb_define_const( cRubyClass, "LED_GREEN", INT2NUM( LED_GREEN ) );
  rb_define_const( cRubyClass, "LED_RED", INT2NUM( LED_RED ) );
  rb_define_const( cRubyClass, "LED_YELLOW", INT2NUM( LED_YELLOW ) );
  rb_define_const( cRubyClass, "LED_BLINK_GREEN", INT2NUM( LED_BLINK_GREEN ) );
  rb_define_const( cRubyClass, "LED_BLINK_RED_YELLOW", INT2NUM( LED_BLINK_RED_YELLOW ) );
  rb_define_const( cRubyClass, "TILT_STATUS_STOPPED", INT2NUM( TILT_STATUS_STOPPED ) );
  rb_define_const( cRubyClass, "TILT_STATUS_LIMIT", INT2NUM( TILT_STATUS_LIMIT ) );
  rb_define_const( cRubyClass, "TILT_STATUS_MOVING", INT2NUM( TILT_STATUS_MOVING ) );
  rb_define_singleton_method( cRubyClass, "new", RUBY_METHOD_FUNC( wrapNew ), 2 );
  rb_define_method( cRubyClass, "inspect", RUBY_METHOD_FUNC( wrapInspect ), 0 );
  rb_define_method( cRubyClass, "close", RUBY_METHOD_FUNC( wrapClose ), 0 );
  rb_define_method( cRubyClass, "read_video", RUBY_METHOD_FUNC( wrapReadVideo ), 0 );
  rb_define_method( cRubyClass, "read_depth", RUBY_METHOD_FUNC( wrapReadDepth ), 0 );
  rb_define_method( cRubyClass, "status?", RUBY_METHOD_FUNC( wrapStatus ), 0 );
  rb_define_method( cRubyClass, "led=", RUBY_METHOD_FUNC( wrapSetLED ), 1 );
  rb_define_method( cRubyClass, "tilt=", RUBY_METHOD_FUNC( wrapSetTilt ), 1 );
  rb_define_method( cRubyClass, "get_state", RUBY_METHOD_FUNC( wrapGetState ), 0 );
  rb_define_method( cRubyClass, "acc", RUBY_METHOD_FUNC( wrapGetAcc ), 0 );
  rb_define_method( cRubyClass, "tilt_status", RUBY_METHOD_FUNC( wrapGetTiltStatus ), 0 );
  return cRubyClass;
}

void KinectInput::deleteRubyObject( void *ptr )
{
  delete (KinectInputPtr *)ptr;
}

VALUE KinectInput::wrapNew( VALUE rbClass, VALUE rbContext, VALUE rbNode )
{
  VALUE rbRetVal = Qnil;
  try {
    KinectContextPtr *context; Data_Get_Struct( rbContext, KinectContextPtr, context );
    KinectInputPtr ptr( new KinectInput( *context, NUM2UINT( rbNode ) ) );
    rbRetVal = Data_Wrap_Struct( rbClass, 0, deleteRubyObject,
                                 new KinectInputPtr( ptr ) );
  } catch ( std::exception &e ) {
    rb_raise( rb_eRuntimeError, "%s", e.what() );
  };
  return rbRetVal;
}

VALUE KinectInput::wrapInspect( VALUE rbSelf )
{
  KinectInputPtr *self; Data_Get_Struct( rbSelf, KinectInputPtr, self );
  string retVal( (*self)->inspect() );
  return rb_str_new( retVal.c_str(), retVal.length() );
}

VALUE KinectInput::wrapClose( VALUE rbSelf )
{
  KinectInputPtr *self; Data_Get_Struct( rbSelf, KinectInputPtr, self );
  (*self)->close();
  return rbSelf;
}

VALUE KinectInput::wrapReadVideo( VALUE rbSelf )
{
  VALUE retVal = Qnil;
  try {
    KinectInputPtr *self; Data_Get_Struct( rbSelf, KinectInputPtr, self );
    FramePtr frame( (*self)->readVideo() );
    retVal = frame->rubyObject();
  } catch ( std::exception &e ) {
    rb_raise( rb_eRuntimeError, "%s", e.what() );
  };
  return retVal;
}

VALUE KinectInput::wrapReadDepth( VALUE rbSelf )
{
  VALUE retVal = Qnil;
  try {
    KinectInputPtr *self; Data_Get_Struct( rbSelf, KinectInputPtr, self );
    FramePtr frame( (*self)->readDepth() );
    retVal = frame->rubyObject();
  } catch ( std::exception &e ) {
    rb_raise( rb_eRuntimeError, "%s", e.what() );
  };
  return retVal;
}

VALUE KinectInput::wrapStatus( VALUE rbSelf )
{
  KinectInputPtr *self; Data_Get_Struct( rbSelf, KinectInputPtr, self );
  return (*self)->status() ? Qtrue : Qfalse;
}

VALUE KinectInput::wrapSetLED( VALUE rbSelf, VALUE rbState )
{
  try {
    KinectInputPtr *self; Data_Get_Struct( rbSelf, KinectInputPtr, self );
    (*self)->setLED( NUM2INT( rbState ) );
  } catch ( exception &e ) {
    rb_raise( rb_eRuntimeError, "%s", e.what() );
  };
  return rbState;
}

VALUE KinectInput::wrapSetTilt( VALUE rbSelf, VALUE rbAngle )
{
  try {
    KinectInputPtr *self; Data_Get_Struct( rbSelf, KinectInputPtr, self );
    (*self)->setTilt( NUM2DBL( rbAngle ) );
  } catch ( exception &e ) {
    rb_raise( rb_eRuntimeError, "%s", e.what() );
  };
  return rbAngle;
}

VALUE KinectInput::wrapGetState( VALUE rbSelf )
{
  try {
    KinectInputPtr *self; Data_Get_Struct( rbSelf, KinectInputPtr, self );
    (*self)->getState();
  } catch ( exception &e ) {
    rb_raise( rb_eRuntimeError, "%s", e.what() );
  };
  return rbSelf;
}

VALUE KinectInput::wrapGetAcc( VALUE rbSelf )
{
  VALUE rbRetVal = Qnil;
  try {
    KinectInputPtr *self; Data_Get_Struct( rbSelf, KinectInputPtr, self );
    rbRetVal = rb_ary_new3( 3, rb_float_new( (*self)->getAcc( 0 ) ),
                            rb_float_new( (*self)->getAcc( 1 ) ),
                            rb_float_new( (*self)->getAcc( 2 ) ) );
  } catch ( exception &e ) {
    rb_raise( rb_eRuntimeError, "%s", e.what() );
  };
  return rbRetVal;
}

VALUE KinectInput::wrapGetTiltStatus( VALUE rbSelf )
{
  VALUE rbRetVal = Qnil;
  try {
    KinectInputPtr *self; Data_Get_Struct( rbSelf, KinectInputPtr, self );
    rbRetVal = INT2NUM( (*self)->getTiltStatus() );
  } catch ( exception &e ) {
    rb_raise( rb_eRuntimeError, "%s", e.what() );
  };
  return rbRetVal;
}

