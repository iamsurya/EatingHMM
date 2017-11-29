%% This file estimates a path from observations, and checks its accuracy.
% It uses training values calculated for the entire dataset using previous code.
% It also loads data from p2110, calculates the most likely path, then 
% calculates metrics for accuracy.

clear
clc

% HMM Data
O = 4;
T = 78;
M = 1;
Q = 2;
load('../../HMMDataset/AllData.mat');
load('../../HMMDataset/EatingHMMParams.mat');

% Output pointers
OPFILE = fopen('C:\\current\\hmmresults.txt','w');
PATHFILE = fopen('C:\\current\\hmmoutpaths.txt','w');

% Confusion Matrix / Accuracy Values 
TTP = 0;
TTN = 0;
TFP = 0;
TFN = 0;

TSTP = 0;
TSTN = 0;
TSFP = 0;
TSFN = 0;

WEIGHT = 20;

fprintf(OPFILE, 'TotalSec\t\tms_ea\tms_non\ttp\ttn\tfp\tfn\tACC\tFile\n');
[sz, ~] = size(DataSequences);
for j = 1 : sz
    input = DataSequences{j,4};
    obs = DataSequences{j,3};
    timeindices = DataSequences{j,2};
    
    ParticipantID = str2num(DataSequences{j,1}(28:31));
    
    B = mixgauss_prob(input, mu, Sigma, mixmat);
    [path] = viterbi_path(prior_, trans_, B);
    
    compare = [obs+1; path]; % Only for looking at in Matlab

    %% Output Paths
    fprintf(PATHFILE, 'P%d: ', ParticipantID);
    for i = 1:length(path)
        fprintf(PATHFILE, '%d',path(i)-1); 
    end
        fprintf(PATHFILE, '\n');
    %% Calculate values for the confusion matrix (segment wise)
% Segment based values
TP = 0;
FP = 0;
TN = 0;
FN = 0;

% Seconds based values
STP = 0;
SFP = 0;
STN = 0;
SFN = 0;

for i = 1 : length(path)
   % True Positive
   if((obs(i)+1 == 2) && path(i) == 2)
       TP = TP + 1;
       STP = STP + (timeindices(2,i)-timeindices(1,i));
   end
   
   % True Negative
   if((obs(i)+1 == 1) && path(i) == 1)
       TN = TN + 1;
       STN = STN + (timeindices(2,i)-timeindices(1,i));
   end
   
   % False Positive
   if((obs(i)+1 == 1) && path(i) == 2)
       FP = FP + 1;
       SFP = SFP + (timeindices(2,i)-timeindices(1,i));
   end
   
   % False Negative
   if((obs(i)+1 == 2) && path(i) == 1)
       FN = FN + 1;
       SFN = SFN + (timeindices(2,i)-timeindices(1,i));
   end
end

ACC = (TP + TN) / (TP + TN + FP + FN);

TTP = TTP + TP;
TTN = TTN + TN;
TFN = TFN + FN;
TFP = TFP + FP;

TSTP = TSTP + STP;
TSTN = TSTN + STN;
TSFP = TSFP + SFP;
TSFN = TSFN + SFN;

% Convert Indices to seconds
STP = STP / 15;
STN = STN / 15;
SFN = SFN / 15;
SFP = SFP / 15;

GTEASec = STP + SFN;
GTNEASec = STN + SFP;

SACC = ((STP * WEIGHT) + STN)/( ((GTEASec)*WEIGHT) + (GTNEASec));

%fprintf(OPFILE, 'P%d\t%d\t%d\t%d\t%d\t%4.2f\n', ParticipantID, TP, TN, FP,
%FN, ACC); 

fprintf(OPFILE, '%5.0f\t%5.0f\t%5.0f\t%5.0f\t%5.0f\t%5.0f\t%5.0f\t%4.2f\t%s\n', GTEASec + GTNEASec, GTEASec, GTNEASec, STP, STN, SFP, SFN, SACC, DataSequences{j,1});
end

GTEASec = TSTP + TSFN;
GTNEASec = TSTN + TSFP;
SACC = ((TSTP * WEIGHT) + TSTN)/( ((GTEASec)*WEIGHT) + (GTNEASec));
fprintf(OPFILE, 'Batch Stats: %5.0f\t%5.0f\t%5.0f\t%5.0f\t%4.2f\n', TSTP, TSTN, TSFP, TSFN, SACC);

fclose(OPFILE);
fclose(PATHFILE);