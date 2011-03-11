/* HornetsEye - Computer Vision with Ruby
 Copyright (C) 2006, 2007, 2008, 2009 Jan Wedekind

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
// #include <boost/array.hpp>
#ifndef NDEBUG
#include <iostream>
#endif
#include "rubytools.hh"
#include "kinectcontext.hh"

using namespace boost;
using namespace std;

VALUE KinectContext::cRubyClass = Qnil;

KinectContext::KinectContext(void) throw (Error):
  m_context(NULL), m_mutex(PTHREAD_MUTEX_INITIALIZER), m_cond(PTHREAD_COND_INITIALIZER),
  m_instances(0)
{
  freenect_init( &m_context, NULL );
  ERRORMACRO( m_context != NULL, Error, , "Initialisation of libfreenect failed" );
}

KinectContext::~KinectContext(void)
{
  close();
}

void KinectContext::addInstance(void)
{
  m_instances++;
  if ( m_instances == 1 ) {
    pthread_create( &m_thread, NULL, staticThreadFunc, this );
  };
}

void KinectContext::removeInstance(void)
{
  m_instances--;
  if ( m_instances == 0 ) {
    pthread_cond_wait( &m_cond, &m_mutex );
  }
}

void KinectContext::close(void)
{
  if ( m_context != NULL ) {
    pthread_join( m_thread, NULL );
    pthread_mutex_destroy( &m_mutex );
    pthread_cond_destroy( &m_cond );
    freenect_shutdown( m_context );
    m_context = NULL;
  };
}

freenect_context *KinectContext::get(void) throw (Error)
{
  ERRORMACRO( m_context != NULL, Error, , "libfreenect is shut down. Did you call "
              "\"close\" before?" );
  return m_context;
}

void KinectContext::lock(void)
{
  pthread_mutex_lock( &m_mutex );
}

void KinectContext::wait(void)
{
  processEvents();
}

void KinectContext::unlock(void)
{
  pthread_mutex_unlock( &m_mutex );
}

void KinectContext::processEvents(void) throw (Error)
{
  ERRORMACRO( freenect_process_events( m_context ) >= 0, Error, , "Error processing "
              "USB events" );
}

string KinectContext::inspect(void) const
{
  ostringstream s;
  s << "KinectContext()";
  return s.str();
}

void KinectContext::threadFunc(void)
{
  bool quit = false;
  while ( !quit ) {
    lock();
    if ( m_instances > 0 )
      processEvents();
    else {
      pthread_cond_signal( &m_cond );
      quit = true;
    };
    unlock();
  };
}

void *KinectContext::staticThreadFunc( void *self )
{
  ((KinectContext *)self)->threadFunc();
  return self;
}

VALUE KinectContext::registerRubyClass( VALUE module )
{
  cRubyClass = rb_define_class_under( module, "KinectContext", rb_cObject );
  rb_define_singleton_method( cRubyClass, "new", RUBY_METHOD_FUNC( wrapNew ), 0 );
  rb_define_method( cRubyClass, "close", RUBY_METHOD_FUNC( wrapClose ), 0 );
  return cRubyClass;
}

void KinectContext::deleteRubyObject( void *ptr )
{
  delete (KinectContextPtr *)ptr;
}

VALUE KinectContext::wrapNew( VALUE rbClass )
{
  VALUE retVal = Qnil;
  try {
    KinectContextPtr ptr( new KinectContext );
    retVal = Data_Wrap_Struct( rbClass, 0, deleteRubyObject,
                               new KinectContextPtr( ptr ) );
  } catch ( std::exception &e ) {
    rb_raise( rb_eRuntimeError, "%s", e.what() );
  };
  return retVal;
}

VALUE KinectContext::wrapClose( VALUE rbSelf )
{
  KinectContextPtr *self; Data_Get_Struct( rbSelf, KinectContextPtr, self );
  (*self)->close();
  return rbSelf;
}

