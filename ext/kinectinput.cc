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

using namespace std;

VALUE KinectInput::cRubyClass = Qnil;

KinectInput::KinectInput( KinectContextPtr context, int node ) throw (Error):
  m_node( node ), m_device( NULL )
{
  ERRORMACRO( freenect_open_device( context->get(), &m_device, node ) >= 0, Error, ,
              "Failed to open Kinect device number " << node );
  freenect_set_video_format( m_device, FREENECT_VIDEO_RGB );
  freenect_set_depth_format( m_device, FREENECT_DEPTH_11BIT );
}

KinectInput::~KinectInput(void)
{
  close();
}

void KinectInput::close(void)
{
  if ( m_device != NULL ) {
    freenect_close_device( m_device );
    m_device = NULL;
  };
  m_node = -1;
}

bool KinectInput::status(void) const
{
  return m_device != NULL;
}

string KinectInput::inspect(void) const
{
  ostringstream s;
  s << "KinectInput( '" << m_node << "' )";
  return s.str();
}
 
void KinectInput::setLED( unsigned char state ) throw (Error)
{
  ERRORMACRO( m_device != NULL, Error, , "Kinect device is not open. "
              "Did you call \"close\" before?" );
  freenect_set_led( m_device, (freenect_led_options)state );
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
  rb_define_singleton_method( cRubyClass, "new", RUBY_METHOD_FUNC( wrapNew ), 2 );
  rb_define_method( cRubyClass, "close", RUBY_METHOD_FUNC( wrapClose ), 0 );
  rb_define_method( cRubyClass, "status?", RUBY_METHOD_FUNC( wrapStatus ), 0 );
  rb_define_method( cRubyClass, "led=", RUBY_METHOD_FUNC( wrapSetLED ), 1 );
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

VALUE KinectInput::wrapClose( VALUE rbSelf )
{
  KinectInputPtr *self; Data_Get_Struct( rbSelf, KinectInputPtr, self );
  (*self)->close();
  return rbSelf;
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


