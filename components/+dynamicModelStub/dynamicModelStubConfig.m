classdef dynamicModelStubConfig < handle

  properties (Constant=true,GetAccess=protected)   
    % Markov process
    % xdot = A*x + B*u
    % x = state [latLonAlt; rotation; latLonAltRate; rotationRate], double 12-by-1
    % u = forcing function, double numInputs-by-1
    % A = sparse double 12-by-12
    % B = sparse double 12-by-numInputs
    A=sparse([zeros(6),eye(6);zeros(6),zeros(6)]); % 6-DOF state transition
%    B=sparse([zeros(6);eye(6)]); % 6-DOF
%    B=sparse([zeros(6);diag([1,1,0,0,0,1])]); % 3-DOF (X,Y,Theta)
    B=sparse([zeros(6);diag([1,1,1,0,0,0])]); % 3-DOF (X,Y,Z)
%    B=sparse([zeros(6);diag([1,1,0,0,0,0])]); % 2-DOF (X,Y)
    
    % Model fidelity parameter given 32*numInputs bits per block
    blocksPerSecond=2;
  end
  
end
