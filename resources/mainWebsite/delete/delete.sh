begin=$(cat delete-fileBEGIN.html);
middle=$(ls -rt ../upload/files | awk '{print "<a href=\"../upload/files/" $0 "\"><div>" $0 "</h5></div>"}')
end=$(cat delete-fileEND.html);

echo "$begin$middle$end"