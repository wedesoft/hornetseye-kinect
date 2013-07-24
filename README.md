hornetseye-kinect
=================
This Ruby extension is for acquiring images using the libfreenect library.

**Author**:       Jan Wedekind
**Copyright**:    2011
**License**:      GPL

Synopsis
--------

This Ruby extension provides the class {Hornetseye::KinectInput} for capturing video frames and depth images with a Microsoft Kinect sensor.

Installation
------------

*hornetseye-kinect* requires libfreenect

    $ sudo aptitude install libboost-dev
    $ git clone https://github.com/OpenKinect/libfreenect.git
    $ cd libfreenect
    $ cmake .
    $ make
    $ sudo make install
    $ cd ..

To install this Ruby extension, use the following command:

    $ sudo gem install hornetseye-kinect

Alternatively you can build and install the Ruby extension from source as follows:

    $ rake
    $ sudo rake install

Usage
-----

Simply run Interactive Ruby:

    $ irb

You can load and use the libfreenect wrappers as shown below. This example demonstrates setting the tilt angle and the LEDs:

    require 'rubygems'
    require 'hornetseye_kinect'
    include Hornetseye
    DELAY = 0.1
    PAUSE = 1.0
    input = KinectInput.new
    [ 20.0, -20.0, 0.0 ].each do |angle|
      t = Time.new.to_f
      input.led = KinectInput::LED_RED
      input.tilt = angle
      while Time.new.to_f < t + DELAY or input.tilt_status != KinectInput::TILT_STATUS_STOPPED
        input.get_state
        printf "acceleration = [%5.2f, %5.2f, %5.2f]        \r", *input.acc
      end
      input.led = KinectInput::LED_GREEN
      t = Time.new.to_f
      while Time.new.to_f < t + PAUSE
        input.get_state
        printf "acceleration = [%5.2f, %5.2f, %5.2f]        \r", *input.acc
      end
    end

Here is another example displaying RGB and the depth image while keeping the tilt angle at zero:

    require 'rubygems'
    require 'hornetseye_kinect'
    require 'hornetseye_xorg'
    include Hornetseye
    input = KinectInput.new
    input.led = KinectInput::LED_RED
    img = MultiArray.ubytergb 1280, 480
    X11Display.show :output => XVideoOutput do
      img[   0 ...  640, 0 ... 480 ] = input.read_video
      img[ 640 ... 1280, 0 ... 480 ] = ( input.read_depth >> 2 ).clip
      input.tilt = 0.0
      input.get_state
      moving = input.tilt_status == KinectInput::TILT_STATUS_MOVING
      input.led = moving ? KinectInput::LED_RED : KinectInput::LED_GREEN
      img
    end

