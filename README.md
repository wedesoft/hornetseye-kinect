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

