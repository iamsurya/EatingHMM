%%

clear
clc

O = 4;
T = 78;
nex = 1;
load('HMM Dataset\2persondata.mat');
M = 1;
Q = 2;
left_right = 0;
data = cell(1,2);
data(1,1) = {p2110};
data(1,2) = {p2111};
prior0 = normalise(rand(Q,1));
transmat0 = mk_stochastic(rand(Q,Q));
%%
[mu0, Sigma0] = mixgauss_init(Q*M, reshape(data, [O T*nex]), 'full', 'kmeans');
mu0 = reshape(mu0, [O Q M]);
Sigma0 = reshape(Sigma0, [O O Q M]);
mixmat0 = ones(Q,1);
%%
[LL, prior1, transmat1, mu1, Sigma1, mixmat1] = ...
    mhmm_em(data, prior0, transmat0, mu0, Sigma0, mixmat0, 'max_iter', 2);

%

B = mixgauss_prob(p2110, mu1, Sigma1, mixmat1);
[path] = viterbi_path(prior1, transmat1, B);