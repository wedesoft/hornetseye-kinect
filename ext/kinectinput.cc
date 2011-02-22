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

KinectInput::KinectInput( KinectContextPtr context, int node ) throw (Error):
  m_node( node ), m_device( NULL )
{
}

KinectInput::~KinectInput(void)
{
  close();
}

void KinectInput::close(void)
{
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
 
VALUE KinectInput::registerRubyClass( VALUE module )
{
}

