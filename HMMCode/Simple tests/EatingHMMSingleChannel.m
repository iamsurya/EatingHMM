%% HMMAccuracy was hard to port to C because it was continuous and has 
% 4 channels (features).
% This file uses just one feature to calculate path and test accuracy of
% the path.
% It uses training values calculated for the entire dataset using previous code.
% It also loads data from p2110, calculates the most likely path, then 
% calculates metrics for accuracy.

clear
clc

O = 4;
T = 78;
nex = 1;
load('../../HMMDataset/2persondata.mat');
load('../../HMMDataset/EatingHMMParams.mat');
M = 1;
Q = 2;


obs = p2110stateseq;

if (1)
    Channel = 4;
    input = p2110(Channel,:);
    mu = mu(Channel,:);
    Sigma1(1,1,1) = Sigma(Channel,Channel,1);
    Sigma1(1,1,2) = Sigma(Channel,Channel,2);

    Sigma = Sigma1;
end

B = mixgauss_prob(input, mu, Sigma, mixmat);
[path] = viterbi_path(prior_, trans_, B);

compare = [obs+1; path]; % Only for looking at in Matlab

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


% Calculate Accuracy and print the results
ACC = (TP + TN) / (TP + TN + FP + FN);
path
fprintf('File\tTP\tTN\tFP\tFN\tACC\n');
fprintf('P2111\t%d\t%d\t%d\t%d\t%4.2f\n', TP, TN, FP, FN, ACC);