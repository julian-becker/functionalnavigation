classdef OpticalFlowOpenCVConfig < handle
  properties (Constant=true,GetAccess=protected)
    displayFlow = false; % (false) display flow results
    isDense = false; % (false) compute flow over all pixels
    windowSize = 9; % (9) search window at each level
    levels = 5; % (5) number of pyramid levels to use
  end
end
