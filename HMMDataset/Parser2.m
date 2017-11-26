% Parser 2. Uses neaData and eaData and creates two matrices for both the
% states (X,4) matrices for both.

neamat = ones(length(neadata), 4);
eamat = ones(length(eadata), 4);

for i = 1 : length(eadata)
    for j = 1:4
    eamat(i,j) = eadata{i,4+j};
    end
end

for i = 1 : length(neadata)
    for j = 1:4
    neamat(i,j) = neadata{i,4+j};
    end
end