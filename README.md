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

*hornetseye-kinect* requires the V4L2 headers. If you are running Debian or (K)ubuntu, you can install them like this:

    $ sudo aptitude install linux-libc-dev

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

