#!/bin/bash

while true; do
  
    begin=$(cat /app/assets/LeuleuWebsite/upload/upload-fileBEGIN.html);
    middle=$(ls -rt /app/uploaded_files/| awk '{print "<h5>" $0 "</h5>"}')
    end=$(cat /app/assets/LeuleuWebsite/upload/upload-fileEND.html);
  
    echo "$begin$middle$end" > /app/assets/LeuleuWebsite/upload/upload-file.html
    
    sleep 0.5
done
