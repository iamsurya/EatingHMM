%% This program calculates the transition matrices from the dataset
clear 
clc
load('../HMM Dataset/HMMData.mat')

%% Assign values to variables 
Q = 2; % States
O = 4; % Observables
M = 1; % Gaussians

IndicesCol = 2;
StateSeqCol = 3;
ObsSeqCol = 4;

prior = zeros(Q,1);
transition = zeros(Q,Q);

DataSequences = cell(1,4);
DataSequences{1,1} = data{1,1}; % First File Name
SCount = 1;

for i = 1 : length(data)
    
    % If Filename changes, add a row to the array
    FileName = data{i,1};
    if(strcmp(FileName, DataSequences{SCount,1}) == 0)
        DataSequences{SCount+1,1} = FileName;
        [SCount ~] = size(DataSequences);
        Name = FileName;
        DataSequences{SCount,StateSeqCol} = [];
        DataSequences{SCount,ObsSeqCol} = [];
    end
    
    % Add the start and end times for the segment
    indices = [data{i,2}; data{i,3}];
    DataSequences{SCount,IndicesCol} = [DataSequences{SCount,IndicesCol} indices];
    % Add the label to the sequence
    DataSequences{SCount,StateSeqCol} = [DataSequences{SCount,StateSeqCol} data{i, 4}];
    % Add the observation to the sequence
    obs = [data{i, 5}; data{i, 6}; data{i, 7}; data{i, 8}];
    DataSequences{SCount,ObsSeqCol} = [DataSequences{SCount,ObsSeqCol} obs];
end

disp('Execution Complete');