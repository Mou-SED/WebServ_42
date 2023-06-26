import cgi


def get_cookie(name):
    """Get the value of a cookie."""
    cookie = cgi.FieldStorage()
    return cookie.getvalue(name)


def set_cookie(name, value, expires=None, path="/", domain=None):
    """Set a cookie."""
    cookie = cgi.Cookie(name=name, value=value,
                        expires=expires, path=path, domain=domain)
    print(cookie)


def main():
    """Main function."""
    username = get_cookie("username")
    if username is None:
        username = "Guest"
    print("Hello, {}!".format(username))
    set_cookie("username", username)


if __name__ == "__main__":
    main()
