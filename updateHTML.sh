#!/bin/bash

begin=$(cat /app/assets/LeuleuWebsite/upload/upload-fileBEGIN.html);
middle=$(ls -rt /app/uploadedFiles/| awk '{print "<a href=\"/uploadedFiles/" $0 "\"><div>" $0 "</h5></div>"}')
end=$(cat /app/assets/LeuleuWebsite/upload/upload-fileEND.html);

echo "$begin$middle$end" > /app/assets/LeuleuWebsite/upload/upload-file.html

begin=$(cat /app/assets/LeuleuWebsite/delete/delete-fileBEGIN.html);
end=$(cat /app/assets/LeuleuWebsite/delete/delete-fileEND.html);

echo "$begin$middle$end" > /app/assets/LeuleuWebsite/delete/delete-file.html
