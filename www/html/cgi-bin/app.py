import os

x = os.getenv("QUERY_STRING")

x.split("&")

name = x.split("&")[0].split("=")[1]
age = x.split("&")[1].split("=")[1]
email = x.split("&")[2].split("=")[1]

a = """
<!DOCTYPE html>
<html>
<head>
  <title>Hello, world!</title>
  <style>
    body {
      font-family: sans-serif;
      text-align: center;
      background-color: #f5f5f5;
      margin: 0;
      padding: 0;
      font-family: monospace;
    }

    .container {
      max-width: 600px;
      margin: 40px auto;
      background-color: #fff;
      padding: 30px 60px;
      border-radius: 10px;
      box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
      display: inline-block;
      text-align: left;
    }

    h1 {
      text-align: center; 
      font-size: 2.5em;
      color: #007bff;
      margin-bottom: 0;
    }

    p {
      font-size: 1.2em;
      color: #333;
    }

    .highlight {
      color: #007bff;
      font-weight: bold;
    }

    h2 {
      text-align: center;
      padding: 0;
      margin-top: 0;
      margin-bottom: 50px;
      font-size: 1.3em;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Hello, world!</h1>
    <h2>This is a simple HTML for testing CGI</h2>
    <p>This is your name: <span class="highlight"> """ + name + """</span></p>
    <p>This is your age: <span class="highlight"> """ + age + """ </span></p>
    <p>Your email: <span class="highlight"> """ + email + """ </span></p>
  </div>
</body>
</html>
"""
print(a)