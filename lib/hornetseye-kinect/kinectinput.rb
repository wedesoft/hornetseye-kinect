# hornetseye-kinect - Kinect sensor
# Copyright (C) 2011 Jan Wedekind
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Namespace of Hornetseye computer vision library
module Hornetseye

  # Class for handling a Kinect sensor
  #
  # This Ruby-extension is based on libfreenect.
  #
  # @see http://openkinect.org/
  class KinectInput

    class << self

      # libfreenect handle
      #
      # @private
      @@context = nil

      # Alias for overriding native method
      #
      # @private
      alias_method :orig_new, :new

      # Open the Kinect sensor
      #
      # @param [Integer] node Camera node to open.
      #
      # return [KinectInput] An object for accessing the Kinect sensor.
      def new( node = 0 )
        context = @@context || KinectContext.new
        begin
          retval = orig_new context, node
          @@context = context
          retval
        ensure
          context.close unless @@context
        end
      end

    end

    alias_method :read, :read_video

    include ReaderConversion

  end

end

