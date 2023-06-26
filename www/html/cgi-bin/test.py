
x = input("")

x.split("&")

fistname = x.split("&")[0].split("=")[1]
age = x.split("&")[1].split("=")[1]

fistname = fistname.replace("+", " ")

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
<p>This is your Fistname = """ + fistname + """
<p> This is your age = """ + str(age) + """
</body>
</html>
"""
print(a)