% We describe a 2 state simple HMM which uses pdf's.
% We have two states described as the Country (Denmark and India)
% We use numbers from the link below and modify them to make the gaussians
% seperable
% http://www.wecare4eyes.com/averageemployeeheights.htm
% Average height for Denmark : 184 cm +- 10 cm (200 +- 7) (179 - 221)
% Average height for Indonesia: 158cm +- 10 cm (150 +- 5) (135 - 165)
% All we can do is observe the heights of the men from a window

clc
clear

% The emission matrix (probability of observation given a state) is
% described as

NumStates = 2;
Q = NumStates;

NumObs = 1;
O = NumObs;

NumGaussians = 1;
M = NumGaussians;

% The means for observations ( O X Q )
obsmeans = [200 150];
obsmeans = (obsmeans - 141)/ 77;
mu0 = reshape(obsmeans, [O Q M]); % Reshape the means into an O X Q X M array

%% The co(variances) (since we have only one) for observations
ObsSigma = [7 4]; % O X Q X M
ObsSigma = [7 4]/77;% O X Q X M
% The number of elements in ObsSigma must be equal to O X O X Q X M
Sigma0 = reshape(ObsSigma, [O O Q M]); % Reshape the covariances into a O * O * Q * M array

%% Some functions in the toolkit need the mixture matrix
%% set to [] or ones(Q,1) if only one mixture component
mixmat0 = ones(Q,M); 
%%
% Observed heights [O X Length]
data = [184   150   155   200   218   148   154   209   141   196];
%min(data)
data = data - min(data);
%max(data)
data  = data / max(data);

%% Prior probability of states Q X 1 Matrix
prior0 = [0.5; 0.5];

% Transition probabilities Q X Q Matrix 
transmat0 = [0.5 0.5; 0.5 0.5]; 

B = mixgauss_prob(data, mu0, Sigma0, mixmat0);
[path] = viterbi_path(prior0, transmat0, B)
