%% This file estimates a path from observations, and checks its accuracy.
% It uses training values calculated for the entire dataset using previous code.
% It also loads data from AllData.mat, and finds data for the specific file specified.
% It then calculates the most likely path, and calculates metrics for accuracy.

clear
clc

FileToFind = 2014;

O = 4;
T = 78;
nex = 1;
load('../../HMMDataset/AllData.mat');
load('../../HMMDataset/EatingHMMParams.mat');
M = 1;
Q = 2;


%%
[sz, ~] = size(DataSequences);
for j = 1 : sz
    if(str2num(DataSequences{j,1}(28:31)) == FileToFind)
       break; 
    end
end
%%
input = DataSequences{j,4};
obs = DataSequences{j,3};

B = mixgauss_prob(input, mu, Sigma, mixmat);
[path] = viterbi_path(prior_, trans_, B);

compare = [obs+1; path]; % Only for looking at in Matlab

%% Output Path
fprintf('File: P%d\n', FileToFind);
fprintf('Path: ');
for i = 1:length(path)
   fprintf('%d',path(i)-1); 
end
fprintf('\n');
%% Calculate values for the confusion matrix
TP = 0;
FP = 0;
TN = 0;
FN = 0;

for i = 1 : length(path)
   % True Positive
   if((obs(i)+1 == 2) && path(i) == 2)
       TP = TP + 1;
   end
   
   % True Negative
   if((obs(i)+1 == 1) && path(i) == 1)
       TN = TN + 1;
   end
   
   % False Positive
   if((obs(i)+1 == 1) && path(i) == 2)
       FP = FP + 1;
   end
   
   % False Negative
   if((obs(i)+1 == 2) && path(i) == 1)
       FN = FN + 1;
   end
end

ACC = (TP + TN) / (TP + TN + FP + FN);

fprintf('File\tTP\tTN\tFP\tFN\tACC\n');
fprintf('P%d\t%d\t%d\t%d\t%d\t%4.2f\n', FileToFind, TP, TN, FP, FN, ACC);