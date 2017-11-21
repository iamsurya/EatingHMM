%% This program parses data from HMM Data and creates a cell array to be used in the HMM program

neadata = {};
eadata = {};

ncount = 1;
ecount = 1;

for i = 1 : length(data)
    if(data{i,4} == 0)
       for j = 1 : 8
       neadata{ncount,j} = data{i,j};
       end
       ncount = ncount + 1;
    end
    if(data{i,4} == 1)
       for j = 1 : 8
       eadata{ecount,j} = data{i,j};
       end
       ecount = ecount + 1;
    end
end

clear ecount
clear ncount
clear i
clear j

