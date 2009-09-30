% NOTES
% Camera and body frame axis order is forward-right-down
% Using SI units (meters, seconds, radians)
% TODO: define exceptions for invalid indices and other errors
classdef camera < sensor
  
  methods (Access=public,Abstract=true)
    % Interpret image layers
    %
    % OUTPUT
    % str = 
    %  'rgbi' red-green-blue-infrared
    %  'rgb'  red-green-blue
    %  'hsv'  hue-saturation-value
    %  'v'    grayscale
    %  'i'    infrared
    str=layers(this);
    
    % Get an image
    %
    % INPUT
    % k = node index, uint32 scalar
    %
    % OUTPUT
    % im = image, uint8 HEIGHT-by-WIDTH-by-LAYERS
    im=getImage(this,k);

    % Get camera frame position and orientation relative to the body frame
    %
    % INPUT
    % k = node index, uint32 scalar
    %
    % OUTPUT
    % posquat = position and orientation in the body frame, double 3-by-1
    posquat=offset(this,k);
    
    % Project ray vectors in the camera frame to image points and vice-versa
    %
    % INPUT/OUTPUT
    % k = node index, uint32 scalar
    % ray = unit vectors in camera frame, double 3-by-P
    % xy = points in pixel coordinates, double 2-by-P
    %
    % NOTES
    % Pixel coordinate interpretation:
    %   x = strides along the non-contiguous dimension (Matlab column minus one)
    %   y = steps along the contiguous dimension (Matlab row minus one)
    % Points outside the image area return NaN-valued vectors
    xy=projection(this,k,ray);
    ray=inverseProjection(this,k,xy);
  end
 
end