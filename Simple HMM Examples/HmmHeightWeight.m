%% We describe a 2 state simple HMM which uses pdf's.
% We have two states described as the Country (Denmark and India)
% We use numbers from the link below and modify them to make the gaussians
% seperable
% http://www.wecare4eyes.com/averageemployeeheights.htm
% Average height for Denmark : 184 cm +- 10 cm (200 +- 7) (179 - 221)
% Average height for Indonesia: 158cm +- 10 cm (150 +- 5) (135 - 165)
% All we can do is observe the heights of the men from a window

% https://en.wikipedia.org/wiki/Human_body_weight
% Denmark (Europe) Weight 71 +- 3
% Indonesia (Asia) Weight 55 +- 2

clc
clear

% The emission matrix (probability of observation given a state) is
% described as

NumStates = 2;
Q = NumStates;

NumObs = 2;
O = NumObs;

NumGaussians = 1;
M = NumGaussians;

% Observed data
heights = [184   150   155   200   218   148   154   209   141   196];
weights = [70   70    70    75   67    48    53    80    52    67];
% Normalize;
hden = max(heights) - min(heights);
hlow = min(heights);
heights = (heights - hlow) / hden;

wden = max(weights) - min(weights);
wlow = min(weights);
weights = (weights - wlow) / wden;

data = [heights; weights];

% Prior probability of states Q X 1 Matrix
prior0 = [0.5; 0.5];

% Transition probabilities Q X Q Matrix 
transmat0 = [0.5 0.5; 0.5 0.5]; 

% The means for observations
obsmeans = [200 150; 71 55]; % O X Q
% Normalize
obsmeans(1,:) = (obsmeans(1,:) - hlow) / hden;
obsmeans(2,:) = (obsmeans(2,:) - wlow) / wden;
mu0 = reshape(obsmeans, [O Q M]); % Reshape the means into an O X Q X M array
%%
% The co(variances) (since we have only one) for observations
% The number of elements in ObsSigma must be equal to O X O X Q X M
ObsSigma = zeros(2,2,2);
ObsSigma(:,:,1) = [7,0;0,3] / hden;
ObsSigma(:,:,2) = [4 0; 0 2] / wden;
%Sigma0 = reshape(ObsSigma, [O O Q M]); % Reshape the covariances into a O * O * Q * M array
Sigma0 = ObsSigma;
%% Some functions in the toolkit need the mixture matrix
%% set to [] or ones(Q,1) if only one mixture component
mixmat0 = ones(Q,M); 

%%
B = mixgauss_prob(data, mu0, Sigma0, mixmat0)
[path] = viterbi_path(prior0, transmat0, B)

