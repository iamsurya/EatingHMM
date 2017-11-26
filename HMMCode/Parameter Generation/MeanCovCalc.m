%% Data parameter calculator
% Calculate the variance, mean and covariance of the features
clear

means = zeros(4,2); % 4 obs x 2 states
sigma = zeros(4,4,2); % 4 obs x 4 obs x 2 states

load('../Hmm Dataset/EANEAMat.mat');

neaSigma = cov(neamat);
eaSigma = cov(eamat );

nmeans = mean(neamat);
emeans = mean(eamat);