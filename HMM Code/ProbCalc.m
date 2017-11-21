%% This program calculates the transition matrices from the dataset
clear 
clc
load('../HMM Dataset/AllData.mat')

% Column details for this data
% 1: Filename string
% 2: [2 x length] Start and End indices
% 3: [1 x length] State Sequence
% 4: [4 x length] Observations Sequence

%% Assign values to variables 
Q = 2; % States
O = 4; % Observables
M = 1; % Gaussians

prior = zeros(Q,1);
transition = zeros(Q,Q);

[sequences ~] = size(DataSequences);

% Calculate Raw Counts for the variables
for S = 1 : sequences
   [~, seqlength] = size(DataSequences{S,3});
    
    % Need to go through each labelled sequence (column 3) and calculate prior probability matrix
   StateSequence = DataSequences{S,3};
   CurrentState = StateSequence(1) + 1; % +1 adjusts for matlab 1 index
   
   prior(CurrentState) = prior(CurrentState) + 1;
   
   for t = 2 : seqlength
       NextState = StateSequence(t) + 1;
       transition(CurrentState, NextState) = transition(CurrentState, NextState) + 1;
       CurrentState = NextState;
   end
end