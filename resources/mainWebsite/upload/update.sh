begin=$(cat upload-fileBEGIN.html);
middle=$(ls -rt files | awk '{print "<a href=\"files/" $0 "\"><div>" $0 "</h5></div>"}')
end=$(cat upload-fileEND.html);

echo "$begin$middle$end"

#begin=$(cat ../../assets/LeuleuWebsite/delete/delete-fileBEGIN.html);
#end=$(cat ../../assets/LeuleuWebsite/delete/delete-fileEND.html);

#echo "$begin$middle$end" > ../../assets/LeuleuWebsite/delete/delete-file.html
#echo "$begin$middle$end"
