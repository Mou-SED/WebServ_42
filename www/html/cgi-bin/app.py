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
}

h1 {
  font-size: 2em;
}

p {
  font-size: 1em;
}
</style>
</head>
<body>
<h1>Hello, world!</h1>
<p>This is a simple HTML for testing CGI</p>
<p>This is your name = """ + name + """
<p> This is your age = """ + str(age) + """
<p> Your email = """ + email + """
</body>
</html>
"""
print(a)