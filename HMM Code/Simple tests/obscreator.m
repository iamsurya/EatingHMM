file = fopen('p2111_gt_path.txt', 'w');
for i = 1 : length(p2110stateseq)
    fprintf(file, '%d\t', p2110stateseq(i));
end
fprintf(file,'\n');
for i = 1 : length(path)
    fprintf(file, '%d\t', path(i)-1);
end
fclose(file);