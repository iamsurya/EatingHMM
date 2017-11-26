file = fopen('p2111obs.txt', 'w');
for j = 1 : 4
for i = 1 : length(p2111stateseq)
    fprintf(file, '%4.6f\t', p2111(j,i));
end
fprintf(file,'\n');
end
fclose(file);