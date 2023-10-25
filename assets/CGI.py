#!/usr/bin/env python3
# Read data from a text file
with open('website_data.txt', 'r') as file:
    website_content = file.read()

# Create an HTML file with the content
html_content = f'''
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Welcome</title>
    <style>
        body {{
background-color: #111;
            color: #fff;
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }}

        .welcome-text {{
font-size: 24px;
            text-align: center;
        }}
    </style>
</head>
<body>
<div class="welcome-text">
    <h1>Welcome to our web-server</h1>
    <p>Leuleu and Tom send their regards</p>
    {website_content}
</div>
</body>
</html>

'''

# Write the HTML content to an HTML file
with open('Leuleu', 'w') as html_file:
    html_file.write(html_content)

print("Website generated successfully in 'index.html' file.")
